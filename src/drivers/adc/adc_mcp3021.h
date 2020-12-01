#ifndef __ADC_MCP3021_H__
#define __ADC_MCP3021_H__

#define ADC_ADDR 0x4D

#define BATTERY_VOLTAGE_READ	1
#define GROVE_VOLTAGE_READ  	0

int adc_init(void);
uint16_t adc_read_val(void);
uint16_t adc_battery_read(void);
uint16_t adc_grove_read(void);


#endif