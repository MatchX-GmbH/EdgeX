#include <stddef.h>
#include <stdint.h>
#include "ioexp.h"
#include "config.h"
#include "gpiohs.h"
#include "fpioa.h"
#include "i2c.h"
#include "stdio.h"
#include "adc_mcp3021.h"

int adc_init(void)
{
	fpioa_set_function(ADC_IO_I2C_SDA_PIN, ADC_IO_I2C_SDA_FUNC);
	fpioa_set_function(ADC_IO_I2C_SCL_PIN, ADC_IO_I2C_SCL_FUNC);

	msleep(1);

    return 0;
}

uint16_t adc_read_val(void)
{
    uint8_t buff[2] = {0,0};
    uint16_t adc_val = 0;
    uint8_t dummy = 0;
    i2c_init(ADC_I2C_DEV, ADC_ADDR, 7, 400 * 1000);
	i2c_recv_data(ADC_I2C_DEV, &dummy, 0, buff, 2);

    //printf("ADC RAW DATA: %u  %u \n\r", buff[0], buff[1]);

    adc_val = ((uint16_t)buff[1] >> 2) | ((uint16_t)buff[0] << 6);

    return adc_val;
}

uint16_t adc_battery_read(void)
{
    uint32_t battery_voltage = 0;

    ioexp_set_val(IOEXP_ADC_CSEL_PIN, BATTERY_VOLTAGE_READ);

    battery_voltage = adc_read_val();

    battery_voltage = (battery_voltage * 5586) / 1000;

    return (uint16_t)battery_voltage;

}

uint16_t adc_grove_read(void)
{
    uint32_t grove_voltage = 0;

    ioexp_set_val(IOEXP_ADC_CSEL_PIN, BATTERY_VOLTAGE_READ);

    grove_voltage  = adc_read_val();

    grove_voltage  = (grove_voltage  * 1852) / 1000;

    return (uint16_t)grove_voltage;
}