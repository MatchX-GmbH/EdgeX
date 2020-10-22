#ifndef ATSHA204A_H
#define ATSHA204A_H

// TODO: this is just an testing stub

static void sha204c_calculate_crc(uint8_t length, uint8_t *data, uint8_t *crc) {
  uint8_t counter;
  uint16_t crc_register = 0;
  uint16_t polynom = 0x8005;
  uint8_t shift_register;
  uint8_t data_bit, crc_bit;

  for (counter = 0; counter < length; counter++) {
    for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1) {
      data_bit = (data[counter] & shift_register) ? 1 : 0;
      crc_bit = crc_register >> 15;
      crc_register <<= 1;
      if (data_bit != crc_bit)
        crc_register ^= polynom;
    }
  }
  crc[0] = (uint8_t) (crc_register & 0x00FF);
  crc[1] = (uint8_t) (crc_register >> 8);
}

void atsha20a4_test_read(){
  fpioa_set_function(15, FUNC_GPIOHS28);
  gpiohs_set_drive_mode(28, GPIO_DM_OUTPUT);
  gpiohs_set_pin(28, 0);
  msleep(1);
  gpiohs_set_pin(28, 1);
  msleep(3);

  fpioa_set_function(15, FUNC_I2C0_SDA);
  fpioa_set_function(14, FUNC_I2C0_SCLK);

  i2c_init(I2C_DEVICE_0, (0xC8 >> 1), 7, 100 * 1000);


  uint8_t read_command[] = {
                            0x03, 7,
                            0x02, 0x0, 0x4, 0,
                            0, 0 };
  sha204c_calculate_crc(5, read_command+1, read_command+6);

  if(!i2c_send_data(I2C_DEVICE_0, read_command, 8)){
    printf("ATSHA204A write failed\n");
  }

  msleep(10);

  uint8_t out[4];
  i2c_recv_data(I2C_DEVICE_0, 0,0, out, 4);

  for(int i = 0; i < 4; i++){
    printf("ATSHA204A %i: 0x%02x\n", i, out[i]);
  }
}

#endif
