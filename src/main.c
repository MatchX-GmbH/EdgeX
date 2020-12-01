#include "sysctl.h"
#include "ai_task.h"
#include "lora_task.h"
#include "config.h"
#include "fpioa.h"
#include "gpio_common.h"
#include "console.h"
#include "dmac.h"
#include "gpiohs.h"
#include "stdio.h"
#include "sysctl.h"
#include "spi.h"
#include "ioexp.h"

#define PLL0_OUTPUT_FREQ 800000000UL
#define PLL1_OUTPUT_FREQ 400000000UL
#define PLL2_OUTPUT_FREQ 45158400UL


#define SPI_SLAVE_INT_PIN       35
#define SPI_SLAVE_INT_IO        4

#define SPI_SLAVE_READY_PIN     34
#define SPI_SLAVE_READY_IO      5
#define SPI_SLAVE_CS_PIN        31
#define SPI_SLAVE_CLK_PIN       32
#define SPI_SLAVE_MOSI_PIN      33
#define SPI_SLAVE_MISO_PIN      33

#define SPI_SLAVE_HI_PIN     30
#define SPI_SLAVE_HI_IO      8

extern uint32_t g_lcd_gram0[640 * 480 / 2];
extern uint32_t g_lcd_gram1[640 * 480 / 2];
//extern uint32_t g_display[640 * 480 / 2];
extern volatile uint8_t g_ram_mux;


int dumb_task( void ){
  while(1){
    //printf("workwork\n");
    //sleep(1);
  }
}

int main(void)
{
    /* Set CPU and dvp clk */
    sysctl_pll_set_freq(SYSCTL_PLL0, PLL0_OUTPUT_FREQ);
    sysctl_pll_set_freq(SYSCTL_PLL1, PLL1_OUTPUT_FREQ);
    sysctl_pll_set_freq(SYSCTL_PLL2, PLL2_OUTPUT_FREQ);

    dmac_init();
    plic_init();
    sysctl_enable_irq();
    rtc_init();

    // Needed to enable debugging
    sysctl_set_power_mode(SYSCTL_POWER_BANK0, SYSCTL_POWER_V18);
    sysctl_set_spi0_dvp_data(1);

    console_register();
    printf("Hello World!!\n\r");

    ioexp_init();
    ioexp_set_val(IOEXP_CAM_EN_PIN, IOEXP_CAM_EN_ON);
    ioexp_set_val(IOEXP_LCD_EN_PIN, IOEXP_LCD_EN_ON);

    //while(1);
    /* Start the AI processing on the second core */
    //ai_task();
    register_core1(ai_task, 0);

    sleep(3);

    /* Execute radio sw */
    //lora_task();
    //register_core1(0, 0);

    //spi_slave_();

    while(1){
      printk("sleep\n\r");
      sleep(1);
    }

    return 0;
}

