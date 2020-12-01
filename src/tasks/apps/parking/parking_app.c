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
#include "config.h"
#include "pconfig.h"
#include "spot_analysis.h"
#include "display_helpers.h"


kpu_model_context_t task;
#define MODEL_SIZE (2800 * 1024) // 2.8 MB
uint8_t model_data[MODEL_SIZE];

// camera image data buffer
uint8_t* ai_image;

// unused for this app
uint8_t* g_lcd_gram0 = 0;
uint8_t* g_lcd_gram1 = 0;

// helper for ai processing
volatile uint8_t ai_done_flag;
static int ai_done(void *ctx)
{
  ai_done_flag = 1;
  return 0;
}


// helper buffer for display
uint8_t slot1[64 * 64 * 2];
uint8_t scaled_image_rgb565[320 * 240 * 2];

// copy of image data to send over the usb
// TODO: check if we can reuse ai_buffer
uint8_t usb_copy[600 * 800] __attribute__((aligned(128)));
bool usb_copy_flag = false;
bool skip_processing = false;

void ai_app_init(){
    ai_image = (uint8_t *)iomem_malloc(800 * 600 * 3);

    printf("Load neural network\n\r");
    w25qxx_init(3, 0);
    w25qxx_enable_quad_mode();
    w25qxx_read_data(0x1400000, model_data, MODEL_SIZE, W25QXX_QUAD_FAST);


    /* LCD init */
    printf("LCD init\n\r");
    lcd_init();
    lcd_set_direction(DIR_YX_LRDU);
    lcd_clear(BLACK);
    lcd_draw_string(136, 70, "HELLO WORLD", WHITE);

    /* DVP init */
    printf("DVP init\n\r");
    dvp_init(8);
    dvp_set_xclk_rate(24000000);
    dvp_disable_burst();
    dvp_set_output_enable(DVP_OUTPUT_AI, 1);
    // disable display data copy
    dvp_set_output_enable(DVP_OUTPUT_DISPLAY, 0);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    dvp_set_image_size(800,600);
    ov2640_init2();
    ov_2640_set_framesize(false, 800, 600, 0, 0, 320, 240);

    dvp_set_ai_addr((uint32_t)ai_image, (uint32_t)(ai_image + 800 * 600), (uint32_t)(ai_image + 800 * 600 * 2));
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_disable_auto();

    /* DVP interrupt config */
    printf("DVP interrupt config\n\r");
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, on_irq_dvp, NULL);
    plic_irq_enable(IRQN_DVP_INTERRUPT);

    /* enable global interrupt */
    sysctl_enable_irq();

    /* system start */
    printf("system start\n\r");
    g_dvp_finish_flag = 0;
    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

    /* init kpu */
    if (kpu_load_kmodel(&task, model_data) != 0)
    {
        printf("\nmodel init error\n\r");
        while (1);
    }

    printf("model loaded\n\r");
}

void ai_app_process(){
    /* dvp finish*/
    while (g_dvp_finish_flag == 0)
      ;

    g_ram_mux ^= 0x01;

    if(usb_copy_flag){
      skip_processing = true;
      const uint8_t* blue_ch = ai_image + 800 * 600;

      uint8_t* copy = (uint8_t*)((size_t)usb_copy - 0x40000000);

      for(int y = 0; y < 600; y++){
        for(int x = 0; x < 800; x++){
          copy[y * 800 + x] = blue_ch[y * 800 + x];
        }
      }
      usb_copy_flag = 0;
    }

    if(!skip_processing)
      process_camera();

    float *output;
    size_t output_size;
    int outs[6];

    for(int i = 0; i < parking_data.spot_count; i++){
          kpu_run_kmodel(&task, parking_data.images[i], DMAC_CHANNEL5, ai_done, NULL);
          while(!ai_done_flag);
          ai_done_flag = 0;
          kpu_get_output(&task, 0, &output, &output_size);
          //printf("output_size %f %u\n", output[0], output_size);
          outs[i] = output[0] > 0.3;
    }

    extern uint8_t scaled_image[320 * 240] __attribute__((aligned(128)));
    convert_to_rgb_big(scaled_image,scaled_image_rgb565);
    lcd_draw_picture(0, 0, 320, 240, scaled_image_rgb565);

    for(int i = 0; i < parking_data.spot_count; i++){
      parking_slot_t* slot = &pconfig.parking_slots[i];
      uint16_t x = slot->x - slot->size, y = slot->y - slot->size, x2 = slot->x + slot->size, y2 = slot->y + slot->size;
      float ratio = 320.f / 1600.f;

      x *= ratio;
      y *= ratio;
      x2 *= ratio;
      y2 *= ratio;

      lcd_draw_rectangle(x, y, x2, y2, 1, outs[i] ? RED : GREEN);
    }

    for(int i = 9990; i < parking_data.spot_count; i++){
      convert_to_rgb(parking_data.images[i], slot1);

      uint16_t x = (i%5) * 64;
      uint16_t y = (i/5) * 64;

      lcd_draw_picture(x, y, 64, 64, slot1);
      lcd_draw_rectangle(x, y, x + 3, y + 3, 1, outs[i] ? RED : GREEN);
    }

    g_dvp_finish_flag = 0;
}

void lora_app_generate_payload(uint8_t* payload, uint8_t* payload_size){
  //TODO: define payload
  *payload_size = 1;
}
