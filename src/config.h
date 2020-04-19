#ifndef __CONFIG_H__
#define __CONFIG_H__

#define HW_VER_1V1
//#include "i2c.h"

#if false

#define SX1261_SPI_NSS_PIN      30
#define SX1261_SPI_NSS_GPIO     10
#define SX1261_SPI_NSS_FUNC     (FUNC_GPIOHS0 + SX1261_SPI_NSS_GPIO)

#define SX1261_SPI_MOSI_PIN     31
#define SX1261_SPI_MOSI_FUNC    FUNC_SPI1_D0

#define SX1261_SPI_MISO_PIN     32
#define SX1261_SPI_MISO_FUNC    FUNC_SPI1_D1

#define SX1261_SPI_SCK_PIN      33
#define SX1261_SPI_SCK_FUNC     FUNC_SPI1_SCLK

#define SX1261_BUSY_PIN         35
#define SX1261_BUSY_GPIO        11
#define SX1261_BUSY_FUNC        (FUNC_GPIOHS0 + SX1261_BUSY_GPIO)

#define SX1261_DIO1_PIN         34
#define SX1261_DIO1_GPIO        12
#define SX1261_DIO1_FUNC        (FUNC_GPIOHS0 + SX1261_DIO1_GPIO)

#define SX1261_RESET_PIN        36
#define SX1261_RESET_GPIO       13
#define SX1261_RESET_FUNC       (FUNC_GPIOHS0 + SX1261_RESET_GPIO)

#define SX1261_FEM_CRX_PIN      37
#define SX1261_FEM_CRX_GPIO     14
#define SX1261_FEM_CRX_FUNC     (FUNC_GPIOHS0 + SX1261_FEM_CRX_GPIO)

#else

#define SX1261_SPI_NSS_PIN      7
#define SX1261_SPI_NSS_GPIO     10
#define SX1261_SPI_NSS_FUNC     (FUNC_GPIOHS0 + SX1261_SPI_NSS_GPIO)

#define SX1261_SPI_MOSI_PIN     9
#define SX1261_SPI_MOSI_FUNC    FUNC_SPI1_D0

#define SX1261_SPI_MISO_PIN     10
#define SX1261_SPI_MISO_FUNC    FUNC_SPI1_D1

#define SX1261_SPI_SCK_PIN      8
#define SX1261_SPI_SCK_FUNC     FUNC_SPI1_SCLK

#define SX1261_BUSY_PIN         12
#define SX1261_BUSY_GPIO        11
#define SX1261_BUSY_FUNC        (FUNC_GPIOHS0 + SX1261_BUSY_GPIO)

#define SX1261_DIO1_PIN         13
#define SX1261_DIO1_GPIO        12
#define SX1261_DIO1_FUNC        (FUNC_GPIOHS0 + SX1261_DIO1_GPIO)

#define SX1261_RESET_PIN        11
#define SX1261_RESET_GPIO       13
#define SX1261_RESET_FUNC       (FUNC_GPIOHS0 + SX1261_RESET_GPIO)

#define SX1261_FEM_CRX_PIN      6
#define SX1261_FEM_CRX_GPIO     14
#define SX1261_FEM_CRX_FUNC     (FUNC_GPIOHS0 + SX1261_FEM_CRX_GPIO)


#define LCD_SPI_SCK_PIN         39
#define LCD_SPI_SCK_FUNC        FUNC_SPI0_SCLK

#define LCD_SPI_SS_PIN          36
#define LCD_SPI_SS_FUNC         FUNC_SPI0_SS3

#define LCD_RST_PIN             37
#define LCD_RST_GPIO            15
#define LCD_RST_FUNC            (FUNC_GPIOHS0 + LCD_RST_GPIO)

#define LCD_DCX_PIN             38
#define LCD_DCX_GPIO            16
#define LCD_DCX_FUNC            (FUNC_GPIOHS0 + LCD_DCX_GPIO)


#define LED_R_PIN               20
#define LED_R_GPIO              17
#define LED_R_FUNC              (FUNC_GPIOHS0 + LED_R_GPIO)

#define LED_G_PIN               19
#define LED_G_GPIO              18
#define LED_G_FUNC              (FUNC_GPIOHS0 + LED_G_GPIO)

#define LED_B_PIN               18
#define LED_B_GPIO              19
#define LED_B_FUNC              (FUNC_GPIOHS0 + LED_B_GPIO)


#define SD_SPI_SS_PIN           20
#define SD_SPI_SS_GPIO          17
#define SD_SPI_SS_FUNC          FUNC_GPIOHS0 + LED_R_GPIO)

#define SD_SPI_CLK_PIN           20
#define SD_SPI_CLK_GPIO          17
#define SD_SPI_CLK_FUNC          FUNC_GPIOHS0 + LED_R_GPIO)

#define SD_SPI_MISO_PIN         19
#define SD_SPI_MISO_GPIO        18
#define SD_SPI_MISO_FUNC        (FUNC_GPIOHS0 + LED_G_GPIO)

#define SD_SPI_MOSI_PIN         18
#define SD_SPI_MOSI_GPIO        19
#define SD_SPI_MOSI_FUNC        (FUNC_GPIOHS0 + LED_B_GPIO)

#endif

#ifdef HW_VER_1V1

#define IOEXP_IO_I2C_SDA_PIN	34
#define IOEXP_IO_I2C_SDA_FUNC	(FUNC_I2C1_SDA)
#define IOEXP_IO_I2C_SCL_PIN	35
#define IOEXP_IO_I2C_SCL_FUNC	(FUNC_I2C1_SCLK)
#define IOEXP_IO_I2C_DEV		I2C_DEVICE_1

#endif


#define RF_FREQUENCY                                868000000 // Hz
#define TX_OUTPUT_POWER                             14        // dBm
#define LORA_BANDWIDTH                              2         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 64 // Define the payload size here

#endif
