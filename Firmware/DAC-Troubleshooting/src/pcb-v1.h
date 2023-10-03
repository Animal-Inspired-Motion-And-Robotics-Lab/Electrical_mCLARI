/*------------------------------------------------------------------------------------------
*File:      pcb-v1.h
*Author:    Parker McDonnell
*Lab:       AIM Robotics Lab
*Company:   University of Colorado Boulder
*
*-------------------------------------------------------------------------------------------*/

#ifndef PCB_V1_H_
#define PROPRIOCEPTOR_H_

// MCU pin definitions

#define MCU_1_WIRE          A0
#define MCU_GPIO0_INT       A1
#define MCU_GPIO1_INT       A2   // MCU_SPARE_1 -- PCB different than Proprioceptor v1.2
#define MCU_NEOPIXEL        A3   // MCU_SPARE_0
#define MCU_RX              RX
#define MCU_TX              TX
#define MCU_I2C_SCL         SCL
#define MCU_I2C_SDA         SDA
#define MCU_SPI_MOSI        MOSI
#define MCU_SPI_MISO        MISO
#define MCU_SPI_SCK         SCK

// GPIO Bottom pin definitions

#define PWR_SHDN_L          0
#define SD_CS               1
#define PWR_ALERT1          2
#define PWR_ALERT2          3
#define CARD_INSERT         4
#define CHG_EN              5
#define BATT_STAT           6
#define DAC_CS              7
#define GPIO_BOTTOM_8       8
#define GPIO_BOTTOM_9       9
#define GPIO_BOTTOM_10      10
#define BRD_ENABLE_1        10
#define GPIO_11             11
#define GPIO_12             12
#define GPIO_13             13
#define RESET               14
#define AUDIO_SD            15

// GPIO Top pin definitions

#define LCD_LIGHT           0
#define LCD_DC              1
#define LCD_RESET           2
#define LCD_CS              3
#define MEM_CS              4
#define TS_CS               5
#define LCD_INT             6
#define LCD_BUSY            7
#define LCD_GPIO_1          8
#define LCD_GPIO_2          9
#define GPIO_SPARE1         10
#define GPIO_SPARE2         11
#define USB_ALERT           12
#define USB_AB_SIDE         13
#define USB_ATTACH          14
#define USB_RESET           15

// GPIO pin definitions

#define HV_ADC_CS              0
#define HV_PWR_ALERT2          1
#define HV_GPIO_SPARE_0        2
#define HV_GPIO_SPARE_1        3
#define HV_EXT_HV_STAT         4
#define HV_ROBOT_CS            5
#define HV_ADH_EN_0            6
#define HV_ADH_EN_1            7
#define HV_ADH_EN_2            8
#define HV_ADC_EN_3            9
#define HV_CS_0                10
#define HV_CP_EN_0             11
#define HV_DAC_CS_0            12
#define HV_BRD_EN              13
#define HV_PWR_SHDN_L          14
#define HV_PWR_ALERT1          15

// HV board devices
#define HV_GPIO_ADDR        0x23
#define HV_PWR_MONITOR_ADDR 0x48

// HV DAC register map
#define HV_DAC_NOP              0x00
#define HV_DAC_DEVICE_ID        0x01
#define HV_DAC_SYNC             0x02
#define HV_DAC_CONFIG           0x03
#define HV_DAC_GAIN             0x04
#define HV_DAC_TRIGGER          0x05
#define HV_DAC_BRDCAST          0x06
#define HV_DAC_DAC_BASE_ADDR    0x08
#define HV_DAC_DAC0             0x08
#define HV_DAC_DAC1             0x09
#define HV_DAC_DAC2             0x0A
#define HV_DAC_DAC3             0x0B
#define HV_DAC_DAC4             0x0C
#define HV_DAC_DAC5             0x0D
#define HV_DAC_DAC6             0x0E
#define HV_DAC_DAC7             0x0F

// Power + Control devices

#define GPIO_TOP_ADDR       0x26
#define GPIO_BOTTOM_ADDR    0x27
#define ADC_ADDR            0x48
#define PWR_MONITOR_ADDR    0x48


#endif /* PCB_V1_H_ */