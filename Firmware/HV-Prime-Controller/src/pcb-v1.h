/*------------------------------------------------------------------------------------------
*File:      pcb-v1.h
*Author:    Parker McDonnell
*Lab:       AIM Robotics Lab
*Company:   University of Colorado Boulder
*
*-------------------------------------------------------------------------------------------*/


#ifndef PCB_V1_H
#define PCB_V1_H

//-------------------------------------------------------------------------------------------
//                                       INCLUDES
//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
//                                        DEFINES
//-------------------------------------------------------------------------------------------


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

// Power + Control devices

#define GPIO_TOP_ADDR       0x26
#define GPIO_BOTTOM_ADDR    0x20 // address for my PCB        //0x27
#define ADC_ADDR            0x48
#define PWR_MONITOR_ADDR    0x48

//-------------------------------------------------------------------------------------------
//                                  EXTERNAL VARIABLES
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//                              PUBLIC FUNCTION PROTOTYPES
//-------------------------------------------------------------------------------------------


#endif /* PCB_V1_H_ */