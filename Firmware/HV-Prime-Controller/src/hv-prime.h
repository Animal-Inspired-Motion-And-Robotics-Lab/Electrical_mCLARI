/*------------------------------------------------------------------------------------------
*File:      hv-prime.h
*Author:    Parker McDonnell
*Lab:       AIM Robotics Lab
*Company:   University of Colorado Boulder
*
*-------------------------------------------------------------------------------------------*/

#ifndef HV_PRIME_H
#define HV_PRIME_H

//-------------------------------------------------------------------------------------------
//                                       INCLUDES
//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
//                                        DEFINES
//-------------------------------------------------------------------------------------------

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


//-------------------------------------------------------------------------------------------
//                                  EXTERNAL VARIABLES
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//                              PUBLIC FUNCTION PROTOTYPES
//-------------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////
//@brief:	
//
//
//@param:   
//@return:  
//@outcome: 
/////////////////////////////////////////////////////////////////////////////////////////////


#endif  //HV_PRIME_H