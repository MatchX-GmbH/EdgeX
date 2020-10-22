#ifndef __IOEXP_H__
#define __IOEXP_H__

#define IOEXP_ADDR 0x24

#define IOEXP_ADC_CSEL_PIN	1
#define IOEXP_CAM_EN_PIN	2
#define IOEXP_LCD_EN_PIN 	4
#define IOEXP_EXP_P3_PIN	8

#define IOEXP_OUT_HI	1
#define IOEXP_OUT_LO	0

#define IOEXP_CAM_EN_ON		1
#define IOEXP_CAM_EN_OFF	0

#define IOEXP_LCD_EN_ON		0
#define IOEXP_LCD_EN_OFF	1



int ioexp_init(void);

void ioexp_set_val(uint8_t pin, uint8_t val);

uint8_t ioexp_read_val(uint8_t pin);


#endif