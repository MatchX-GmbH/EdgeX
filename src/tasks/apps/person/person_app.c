#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <plic.h>
#include <dvp.h>
#include <sysctl.h>
#include <kpu.h>

#include "../common.h"
#include "board_config.h"
#include "nt35310.h"
#include "w25qxx.h"
#include "lcd.h"
#include "ov2640.h"
#include "config.h"
#include "pconfig.h"
#include "region_layer.h"


kpu_model_context_t task;
#define MODEL_SIZE (2800 * 1024) // 2.8 MB
uint8_t model_data[MODEL_SIZE] __attribute__((aligned(256)));

// camera image data buffer
uint8_t* ai_image;

uint8_t* g_lcd_gram0;
uint8_t* g_lcd_gram1;

#define ANCHOR_NUM 5
float g_anchor[ANCHOR_NUM * 2] = {1.08, 1.19, 3.42, 4.41, 6.63, 11.38, 9.42, 5.11, 16.62, 10.52};
static region_layer_t detect_rl;

// helper for ai processing
volatile uint8_t ai_done_flag;
static int ai_done(void *ctx)
{
  ai_done_flag = 1;
  return 0;
}

// ai result
uint8_t ai_person_count;

static void ram_draw_box(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t width, uint16_t color){

  uint16_t * lcd_gram = g_ram_mux ? g_lcd_gram0 : g_lcd_gram1;

  for(int x = x1; x < x2; x++){
    int x_ = x + (x%2 ? -1 : 1);
    lcd_gram[y1 * 320 + x_] = color;
  }
  for(int x = x1; x < x2; x++){
    int x_ = x + (x%2 ? -1 : 1);
    lcd_gram[y2 * 320 + x_] = color;
  }

  x1 += x1%2 ? -1 : 1;
  x2 += x2%2 ? -1 : 1;

  for(int y = y1; y < y2; y++){
    lcd_gram[y * 320 + x1] = color;
  }
  for(int y = y1; y < y2; y++){
    lcd_gram[y * 320 + x2] = color;
  }

}

static void drawboxes(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t class, float prob)
{
  if (x1 >= 320)
    x1 = 319;
  if (x2 >= 320)
    x2 = 319;
  if (y1 >= 240)
    y1 = 239;
  if (y2 >= 240)
    y2 = 239;

  lcd_draw_rectangle(x1, y1, x2, y2, 2, RED);
}

void ai_app_init(){
    g_lcd_gram0 = (uint32_t *)iomem_malloc(320 * 240 * 2);
    g_lcd_gram1 = (uint32_t *)iomem_malloc(320 * 240 * 2);
    ai_image = (uint8_t *)iomem_malloc(320 * 240 * 3);

    printf("Load neural network\n\r");
    w25qxx_init(3, 0);
    w25qxx_enable_quad_mode();
    w25qxx_read_data(0x800000, model_data, MODEL_SIZE, W25QXX_QUAD_FAST);


    /* LCD init */
    printf("LCD init\r\n");
    lcd_init();
    lcd_set_direction(DIR_YX_LRUD);
    lcd_clear(BLACK);
    lcd_draw_string(136, 70, "HELLO WORLD", WHITE);

    /* DVP init */
    printf("DVP init\r\n");
    dvp_init(8);
    dvp_set_xclk_rate(24000000);
    dvp_enable_burst();
    dvp_set_output_enable(DVP_OUTPUT_AI, 1);
    dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    dvp_set_image_size(320,240);
    ov2640_init();

    dvp_set_ai_addr((uint32_t)ai_image, (uint32_t)(ai_image + 320 * 240), (uint32_t)(ai_image + 320 * 240 * 2));
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_disable_auto();

    /* DVP interrupt config */
    printf("DVP interrupt config\r\n");
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, on_irq_dvp, NULL);
    plic_irq_enable(IRQN_DVP_INTERRUPT);

    /* enable global interrupt */
    sysctl_enable_irq();

    /* system start */
    printf("system start\r\n");
    g_dvp_finish_flag = 0;
    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

    /* init kpu */
    if (kpu_load_kmodel(&task, model_data) != 0)
    {
        printf("\nmodel init error\r\n");
        while (1);
    }

    printf("model loaded\r\n");

    detect_rl.anchor_number = ANCHOR_NUM;
    detect_rl.anchor = g_anchor;
    detect_rl.threshold = 0.9;
    detect_rl.nms_value = 0.2;
    region_layer_init(&detect_rl, 10, 7, 125, 320, 240);
}

void ai_app_process(){
    /* dvp finish*/
    while (g_dvp_finish_flag == 0)
      ;

    kpu_run_kmodel(&task, ai_image, DMAC_CHANNEL5, ai_done, NULL);
    while(!ai_done_flag);
    ai_done_flag = 0;

    float *output;
    size_t output_size;
    kpu_get_output(&task, 0, &output, &output_size);
    detect_rl.input = output;
    region_layer_run(&detect_rl, NULL);

    uint32_t * lcd_gram = g_ram_mux ? g_lcd_gram0 : g_lcd_gram1;
    g_ram_mux ^= 0x01;

    lcd_draw_picture(0, 0, 320, 240, (uint8_t*)lcd_gram);
    region_layer_draw_boxes(&detect_rl, drawboxes);

    g_dvp_finish_flag = 0;
}

void lora_app_generate_payload(uint8_t* payload, uint8_t* payload_size){

    *payload_size = 2;
    payload[0] = ai_person_count;

    if(ai_person_count == 1){
        gpiohs_set_pin(LED_R_GPIO, GPIO_PV_LOW);
        gpiohs_set_pin(LED_G_GPIO, GPIO_PV_HIGH);
        gpiohs_set_pin(LED_B_GPIO, GPIO_PV_LOW);
    }else if (ai_person_count == 0){
        gpiohs_set_pin(LED_R_GPIO, GPIO_PV_LOW);
        gpiohs_set_pin(LED_G_GPIO, GPIO_PV_LOW);
        gpiohs_set_pin(LED_B_GPIO, GPIO_PV_HIGH);
    }else{
        gpiohs_set_pin(LED_R_GPIO, GPIO_PV_HIGH);
        gpiohs_set_pin(LED_G_GPIO, GPIO_PV_HIGH);
        gpiohs_set_pin(LED_B_GPIO, GPIO_PV_LOW);
    }
}
