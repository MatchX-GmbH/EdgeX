#include "sysctl.h"
#include "ai_task.h"
#include "lora_task.h"
#include "atomic.h"
#include "config.h"
#include "gpio_common.h"



#define PLL0_OUTPUT_FREQ 800000000UL
#define PLL1_OUTPUT_FREQ 400000000UL
#define PLL2_OUTPUT_FREQ 45158400UL

int main(void)
{
    /* Set CPU and dvp clk */
    sysctl_pll_set_freq(SYSCTL_PLL0, PLL0_OUTPUT_FREQ);
    sysctl_pll_set_freq(SYSCTL_PLL1, PLL1_OUTPUT_FREQ);
    sysctl_pll_set_freq(SYSCTL_PLL2, PLL2_OUTPUT_FREQ);

    plic_init();
    sysctl_enable_irq();
    uarths_init();

    // Needed to enable debugging
    sysctl_set_power_mode(SYSCTL_POWER_BANK0, SYSCTL_POWER_V18);
    sysctl_set_spi0_dvp_data(1);

    /* Start the AI processing on the second core */
    register_core1(ai_task, 0);

    /* Execute radio sw */
    lora_task();

    return 0;
}

