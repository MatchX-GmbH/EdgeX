#include "board_config.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "w25qxx.h"
#include "dvp.h"
#include "sysctl.h"
#include "fpioa.h"
#include "nt35310.h"
#include "lcd.h"
#include "region_layer.h"
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX
#include "incbin.h"
#include "config.h"

#if OV5640
#include "ov5640.h"
#endif
#if OV2640
#include "ov2640.h"
#endif

#define CLASS_NUMBER 20
//INCBIN(model, "/home/kuba/sw/ai-node-fw-stdln/src/tasks/ai/yolo.kmodel");
#define MODEL_SIZE (1400 * 1024) // 1.4 MB
uint8_t model_data[MODEL_SIZE];

kpu_model_context_t task;
static region_layer_t detect_rl;

volatile uint8_t g_ai_done_flag;

static int ai_done(void *ctx)
{
    g_ai_done_flag = 1;
    return 0;
}

uint32_t g_lcd_gram0[38400 * 4] __attribute__((aligned(64)));
uint32_t g_lcd_gram1[38400 * 4] __attribute__((aligned(64)));

uint8_t g_ai_buf[320 * 240 *3 * 4] __attribute__((aligned(128)));

#define ANCHOR_NUM 5

float g_anchor[ANCHOR_NUM * 2] = {1.08, 1.19, 3.42, 4.41, 6.63, 11.38, 9.42, 5.11, 16.62, 10.52};

volatile uint8_t g_dvp_finish_flag = 0;
volatile uint8_t g_ram_mux = 0;
uint8_t ai_person_count = 0;

static int on_irq_dvp(void* ctx)
{
    if (dvp_get_interrupt(DVP_STS_FRAME_FINISH))
    {
        /* switch gram */
        dvp_set_display_addr(g_ram_mux ? (uint32_t)g_lcd_gram0 : (uint32_t)g_lcd_gram1);

        dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
        g_dvp_finish_flag = 1;
    }
    else
    {
        if(g_dvp_finish_flag == 0)
            dvp_start_convert();
        dvp_clear_interrupt(DVP_STS_FRAME_START);
    }

    return 0;
}

#if BOARD_LICHEEDAN
static void io_mux_init(void)
{
    /* Init DVP IO map and function settings */
    fpioa_set_function(42, FUNC_CMOS_RST);
    fpioa_set_function(44, FUNC_CMOS_PWDN);
    fpioa_set_function(46, FUNC_CMOS_XCLK);
    fpioa_set_function(43, FUNC_CMOS_VSYNC);
    fpioa_set_function(45, FUNC_CMOS_HREF);
    fpioa_set_function(47, FUNC_CMOS_PCLK);
    fpioa_set_function(41, FUNC_SCCB_SCLK);
    fpioa_set_function(40, FUNC_SCCB_SDA);

    /* Init SPI IO map and function settings */
    fpioa_set_function(38, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(36, FUNC_SPI0_SS3);
    fpioa_set_function(39, FUNC_SPI0_SCLK);
    fpioa_set_function(37, FUNC_GPIOHS0 + RST_GPIONUM);

    sysctl_set_spi0_dvp_data(1);
}

static void io_set_power(void)
{
    /* Set dvp and spi pin to 1.8V */
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, SYSCTL_POWER_V18);
}

#else
static void io_mux_init(void)
{
    /* Init DVP IO map and function settings */
    fpioa_set_function(11, FUNC_CMOS_RST);
    fpioa_set_function(13, FUNC_CMOS_PWDN);
    fpioa_set_function(14, FUNC_CMOS_XCLK);
    fpioa_set_function(12, FUNC_CMOS_VSYNC);
    fpioa_set_function(17, FUNC_CMOS_HREF);
    fpioa_set_function(15, FUNC_CMOS_PCLK);
    fpioa_set_function(10, FUNC_SCCB_SCLK);
    fpioa_set_function(9, FUNC_SCCB_SDA);

    /* Init SPI IO map and function settings */
    fpioa_set_function(8, FUNC_GPIOHS0 + DCX_GPIONUM);
    fpioa_set_function(6, FUNC_SPI0_SS3);
    fpioa_set_function(7, FUNC_SPI0_SCLK);

    sysctl_set_spi0_dvp_data(1);
}

static void io_set_power(void)
{
    /* Set dvp and spi pin to 1.8V */
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, SYSCTL_POWER_V18);
    sysctl_set_power_mode(SYSCTL_POWER_BANK2, SYSCTL_POWER_V18);
}
#endif

typedef struct
{
  char *str;
  uint16_t color;
  uint16_t height;
  uint16_t width;
  uint32_t *ptr;
} class_label_t;

class_label_t class_label[CLASS_NUMBER] =
  {
   {"aeroplane", GREEN},
   {"bicycle", GREEN},
   {"bird", GREEN},
   {"boat", GREEN},
   {"bottle", 0xF81F},
   {"bus", GREEN},
   {"car", GREEN},
   {"cat", GREEN},
   {"chair", 0xFD20},
   {"cow", GREEN},
   {"diningtable", GREEN},
   {"dog", GREEN},
   {"horse", GREEN},
   {"motorbike", GREEN},
   {"person", 0xF800},
   {"pottedplant", GREEN},
   {"sheep", GREEN},
   {"sofa", GREEN},
   {"train", GREEN},
   {"tvmonitor", 0xF9B6}
  };

static uint32_t label_string_draw_ram[115 * 16 * 8 / 2];


static void label_init(void)
{
  uint8_t index;

  class_label[0].height = 16;
  class_label[0].width = 8 * strlen(class_label[0].str);
  class_label[0].ptr = label_string_draw_ram;
  lcd_ram_draw_string(class_label[0].str, class_label[0].ptr, BLACK, class_label[0].color);
  for (index = 1; index < CLASS_NUMBER; index++) {
    class_label[index].height = 16;
    class_label[index].width = 8 * strlen(class_label[index].str);
    class_label[index].ptr = class_label[index - 1].ptr + class_label[index - 1].height * class_label[index - 1].width / 2;
    lcd_ram_draw_string(class_label[index].str, class_label[index].ptr, BLACK, class_label[index].color);
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

  lcd_draw_rectangle(x1, y1, x2, y2, 2, class_label[class].color);
  lcd_draw_picture(x1 + 1, y1 + 1, class_label[class].width, class_label[class].height, class_label[class].ptr);
}

void ai_task(void){
    io_mux_init();
    io_set_power();
    label_init();

    printf("Load neural network\n");
    w25qxx_init(3, 0);
    w25qxx_enable_quad_mode();
    w25qxx_read_data(0x800000, model_data, MODEL_SIZE, W25QXX_QUAD_FAST);


    /* LCD init */
    printf("LCD init\n");
    lcd_init();
    lcd_set_direction(DIR_YX_LRDU);
    lcd_clear(BLACK);
    lcd_draw_string(136, 70, "DEMO 1", WHITE);

    /* DVP init */
    printf("DVP init\n");
    #if OV5640
    dvp_init(16);
    dvp_set_xclk_rate(50000000);
    dvp_enable_burst();
    dvp_set_output_enable(0, 1);
    dvp_set_output_enable(1, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    dvp_set_image_size(640, 480);
    ov5640_init();
    #else
    dvp_init(8);
    dvp_set_xclk_rate(24000000);
    dvp_enable_burst();
    dvp_set_output_enable(0, 1);
    dvp_set_output_enable(1, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);
    dvp_set_image_size(320, 240);
    ov2640_init();
    #endif

    dvp_set_ai_addr((uint32_t)g_ai_buf, (uint32_t)(g_ai_buf + 320 * 240), (uint32_t)(g_ai_buf + 320 * 240 * 2));
    dvp_set_display_addr((uint32_t)g_lcd_gram0);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_disable_auto();

    /* DVP interrupt config */
    printf("DVP interrupt config\n");
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, on_irq_dvp, NULL);
    plic_irq_enable(IRQN_DVP_INTERRUPT);

    /* enable global interrupt */
    sysctl_enable_irq();

    /* system start */
    printf("system start\n");
    g_ram_mux = 0;
    g_dvp_finish_flag = 0;
    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);

    /* init kpu */
    if (kpu_load_kmodel(&task, model_data) != 0)
    {
        printf("\nmodel init error\n");
        while (1);
    }

    detect_rl.anchor_number = ANCHOR_NUM;
    detect_rl.anchor = g_anchor;
    detect_rl.threshold = 0.4;
    detect_rl.nms_value = 0.3;
    region_layer_init(&detect_rl, 10, 7, 125, 320, 240);

    while (1)
    {
        /* dvp finish*/
        while (g_dvp_finish_flag == 0)
            ;

        /* start to calculate */
        kpu_run_kmodel(&task, g_ai_buf, DMAC_CHANNEL5, ai_done, NULL);
        while(!g_ai_done_flag);
        g_ai_done_flag = 0;

        float *output;
        size_t output_size;
        kpu_get_output(&task, 0, &output, &output_size);
        detect_rl.input = output;

        /* start region layer */
        region_layer_run(&detect_rl, NULL);

        /* display pic*/
        uint32_t * lcd_gram = g_ram_mux ? g_lcd_gram0 : g_lcd_gram1;
        uint8_t * lcd_gram_ = (uint8_t*)lcd_gram;

        g_ram_mux ^= 0x01;
        lcd_draw_picture(0, 0, 320, 240, lcd_gram);
        g_dvp_finish_flag = 0;

        /* draw boxs */
        region_layer_draw_boxes(&detect_rl, drawboxes);
    }

}
