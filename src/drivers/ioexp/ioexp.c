#include <stddef.h>
#include <stdint.h>
#include "ioexp.h"
#include "config.h"
#include "gpiohs.h"
#include "fpioa.h"
#include "i2c.h"
#include "stdio.h"


uint8_t _ioexp_status = 0;

int ioexp_init(void)
{

	// fpioa_set_function(15, FUNC_GPIOHS28);
	// gpiohs_set_drive_mode(28, GPIO_DM_OUTPUT);
	// gpiohs_set_pin(28, 0);
	// msleep(1);
	// gpiohs_set_pin(28, 1);
	// msleep(3);

	fpioa_set_function(IOEXP_IO_I2C_SDA_PIN, IOEXP_IO_I2C_SDA_FUNC);
	fpioa_set_function(IOEXP_IO_I2C_SCL_PIN, IOEXP_IO_I2C_SCL_FUNC);

	msleep(1);

return 0;
}

void ioexp_set_val(uint8_t pin, uint8_t val)
{

	if(val == 0)
	{

		_ioexp_status &= ~pin; 
	}
	else if(val == 1)
	{
		_ioexp_status |= pin; 
	}
	else
	{
		return;
	}
	printf("ioexp_read_val: writing value: %u \n", _ioexp_status);
	i2c_init(IOEXP_IO_I2C_DEV, IOEXP_ADDR, 7, 400 * 1000);
	i2c_send_data(IOEXP_IO_I2C_DEV, &_ioexp_status, 1);

}

uint8_t ioexp_read_val(uint8_t pin)
{




}





