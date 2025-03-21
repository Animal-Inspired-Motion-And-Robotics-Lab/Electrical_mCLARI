#include <Arduino.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SPI.h>
#include <elapsedMillis.h>
#include <SerialCommand.h>
// put function declarations here:

//-------------------------------------------------------------------------------------------
//                                       INCLUDES
//-------------------------------------------------------------------------------------------

#include "pcb-v1.h"
#include "hv-prime.h"
#include "WaveGenerator.h"

// led functions
uint32_t Wheel(byte WheelPos);
void rainbowCycle(uint8_t wait);

// hv dac functions
void dac_write_reg(uint8_t channel, uint16_t value);
uint16_t dac_read_reg(uint8_t addr);

//void dac_clear_all(void);
//void dac_write_all(uint16_t value);
void dac_print(void);
void dac_set_output(uint8_t channel, uint16_t value);
void dac_clear_output(void);

// hv functions 
void hv_disable(void);
void hv_enable(void);
void enable_electroadhesion(void);
void disable_electroadhesion(void);
void read_hv(void);
// command functions
SerialCommand serialCmd;

bool robot_enabled = false;

void disable_robot(void);
void enable_robot(void);
void help(void);
// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

Adafruit_MCP23X17 mcp_top;
Adafruit_MCP23X17 mcp_bottom;
Adafruit_MCP23X17 mcp_hv;

// leg waveform objects

WaveGenerator leg_1(5,200,100,90,500);
WaveGenerator leg_2(5,200,100,90,500);
WaveGenerator leg_3(5,200,100,90,500);
WaveGenerator leg_4(5,200,100,90,500);

// leg current lift/swing variables

uint16_t leg_1_lift = 0;
uint16_t leg_1_swing = 0;
uint16_t leg_2_lift = 0;
uint16_t leg_2_swing = 0;
uint16_t leg_3_lift = 0;
uint16_t leg_3_swing = 0;
uint16_t leg_4_lift = 0;
uint16_t leg_4_swing = 0;

// loop control variables
elapsedMillis loopTimeElapsed = 0;
elapsedMillis runTimeElapsed = 0;
uint32_t loop_period      = 1; // ms   
uint32_t run_time         = 5000; // ms 

bool ramp_up_finished = false;
uint16_t ramp_length = 500; // ms
// NeoPixel Objects

bool first_time = true;

#define WIRE Wire
// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN MCU_NEOPIXEL  
// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  16
// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);


void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  WIRE.begin();
  SPI.begin();
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(115200);

  // initialize the MCP23017 pin expander on all boards
  mcp_bottom.begin_I2C(GPIO_BOTTOM_ADDR);
  mcp_top.begin_I2C(GPIO_TOP_ADDR);
  mcp_hv.begin_I2C(HV_GPIO_ADDR);

  // turn on the high voltage board 
  mcp_bottom.pinMode(BRD_ENABLE_1, OUTPUT);
  mcp_bottom.digitalWrite(BRD_ENABLE_1, LOW);
  delay(1000);
  mcp_bottom.digitalWrite(BRD_ENABLE_1, HIGH);
  // enable the power monitor on the PCB V1
  mcp_bottom.pinMode(PWR_SHDN_L, OUTPUT);
  mcp_bottom.digitalWrite(PWR_SHDN_L, HIGH);

  // enable the HV boost converter charge pump
  mcp_hv.pinMode(HV_CP_EN_0, OUTPUT);
  mcp_hv.digitalWrite(HV_CP_EN_0, LOW);
  // make sure the HV boost converter is disabled
  mcp_hv.pinMode(HV_CS_0, OUTPUT);
  mcp_hv.digitalWrite(HV_CS_0, HIGH);
  //hv_disable();
  mcp_hv.pinMode(HV_ADC_CS, OUTPUT);
  mcp_hv.digitalWrite(HV_ADC_CS, HIGH);
  //
  mcp_bottom.pinMode(DAC_CS, OUTPUT);
  mcp_bottom.digitalWrite(DAC_CS, HIGH);

  // set DAC CS to output
  mcp_hv.pinMode(HV_DAC_CS_0, OUTPUT);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);
  // clear output of all channels

  mcp_hv.pinMode(HV_ADH_EN_0, OUTPUT);
  mcp_hv.digitalWrite(HV_ADH_EN_0, LOW);
  mcp_hv.pinMode(HV_ADH_EN_1, OUTPUT);
  mcp_hv.digitalWrite(HV_ADH_EN_1, LOW);
  mcp_hv.pinMode(HV_ADH_EN_2, OUTPUT);
  mcp_hv.digitalWrite(HV_ADH_EN_2, LOW);
  mcp_hv.pinMode(HV_ADH_EN_3, OUTPUT);
  mcp_hv.digitalWrite(HV_ADH_EN_3, LOW);


  dac_write_reg(HV_DAC_TRIGGER, 0x000A);  // soft reset DAC
  dac_write_reg(HV_DAC_SYNC, 0x0000);     // set device to async mode
  dac_write_reg(HV_DAC_CONFIG, 0x0000);   // no CRC, DO enabled, all DACs enabled, dissable internal VREF
  dac_write_reg(HV_DAC_GAIN, 0x01FF);     // set gain to 2 and ref to 1/2 divider

  // intialize each leg control object
  leg_1.begin();
  leg_2.begin();
  leg_3.begin();
  leg_4.begin();

  // set mode to lift/swing
  leg_1.setMode(0);
  leg_2.setMode(0);
  leg_3.setMode(0);
  leg_4.setMode(0);

  // command functions

  serialCmd.addCommand("on", enable_robot);              // starts sending telemetry
  serialCmd.addCommand("off", disable_robot);              // starts sending telemetry
  serialCmd.addCommand("help",help);
  serialCmd.addCommand("enable_electro",enable_electroadhesion); // toggles electroadhesion channels
  serialCmd.addCommand("disable_electro",disable_electroadhesion); // toggles electroadhesion channels
  serialCmd.addCommand("read",read_hv); // reads the HV TX register
}

void loop() 
{


// run code here
  if(loopTimeElapsed > loop_period)
  {
    
    serialCmd.readSerial();
    loopTimeElapsed = 0;

    if(robot_enabled)
    {
      // run each leg controller and get the new lift/swing values
      leg_1.run(&leg_1_lift, &leg_1_swing, true);
      leg_2.run(&leg_2_lift, &leg_2_swing, true);
      leg_3.run(&leg_3_lift, &leg_3_swing, true);
      leg_4.run(&leg_4_lift, &leg_4_swing, true);
      // write the new values to the DAC
      dac_set_output(0, leg_1_lift);
      dac_set_output(1, leg_1_swing);
      dac_set_output(2, leg_2_lift);
      dac_set_output(3, leg_2_swing);
      dac_set_output(4, leg_3_lift);
      dac_set_output(5, leg_3_swing);
      dac_set_output(6, leg_4_lift);
      dac_set_output(7, leg_4_swing);

    }
    // else
    // {
    //   rainbowCycle(10);
    // }
    }

}

void help()
{
    Serial.println("\nMSG  >> Available Commands:");
     Serial.println("--------------------------------------------------------------------------------------------------------");
    Serial.println("#)      <command>               <argument 1/2/3..N>            <Description>");
    Serial.println("1)      <on>                                               : turn on robot");
    Serial.println("2)      <off>                                              : turn off robot");
    Serial.println("3)      <enable_electro>                                   : enables electroadhesion channels");
    Serial.println("4)      <disable_electro>                                  : disables electroadhesion channels");
    Serial.println("5)      <read>                                             : reads TX register from HV56020");
  }

void enable_robot(void)
{
  Serial.println("Enabling HV supply");
  robot_enabled = true;
  hv_enable();

}

void disable_robot(void)
{
  Serial.println("Disabling HV supply");
  robot_enabled = false;
  hv_disable();
}

void enable_electroadhesion(void)
{
  mcp_hv.digitalWrite(HV_ADH_EN_0, LOW);
  mcp_hv.digitalWrite(HV_ADH_EN_1, LOW);
  mcp_hv.digitalWrite(HV_ADH_EN_2, LOW);
  mcp_hv.digitalWrite(HV_ADH_EN_3, LOW);
  Serial.println("Electroadhesion enabled");


}

void disable_electroadhesion(void)
{
  mcp_hv.digitalWrite(HV_ADH_EN_0, HIGH);
  mcp_hv.digitalWrite(HV_ADH_EN_1, HIGH);
  mcp_hv.digitalWrite(HV_ADH_EN_2, HIGH);
  mcp_hv.digitalWrite(HV_ADH_EN_3, HIGH);
  Serial.println("Electroadhesion disabled");
}
void hv_enable(void)
{
  mcp_hv.digitalWrite(HV_CP_EN_0, HIGH);

  mcp_hv.digitalWrite(HV_CS_0, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  //SPI.transfer16(0x00F4);
  SPI.transfer16(0x00A4);
  SPI.endTransaction();
  mcp_hv.digitalWrite(HV_CS_0, HIGH);

}

void hv_disable(void)
{
  // disable the charge pump
  mcp_hv.digitalWrite(HV_CP_EN_0, LOW);
  // enable the DC-DC converter
  mcp_hv.digitalWrite(HV_CS_0, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  SPI.transfer16(0x00AA); // shutdown converter
  SPI.endTransaction();
  mcp_hv.digitalWrite(HV_CS_0, HIGH);

}

void read_hv(void)
{
  uint16_t reg_value = 0;

  mcp_hv.digitalWrite(HV_CS_0, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  reg_value = SPI.transfer16(0x00EA); // shutdown converter
  SPI.endTransaction();
  mcp_hv.digitalWrite(HV_CS_0, HIGH);

  Serial.print("HV Register Value: 0x");
  Serial.println(reg_value, HEX);
}


void dac_print(void)
{
  Serial.println("DAC Registers:");
  Serial.print("Device ID: ");
  Serial.println(dac_read_reg(HV_DAC_DEVICE_ID));
  Serial.print("Sync: ");
  Serial.println(dac_read_reg(HV_DAC_SYNC));
  Serial.print("Gain: ");
  Serial.println(dac_read_reg(HV_DAC_GAIN));
  Serial.print("Trigger: ");
  Serial.println(dac_read_reg(HV_DAC_TRIGGER));
  Serial.print("Broadcast: ");
  Serial.println(dac_read_reg(HV_DAC_BRDCAST));
}

void dac_write_reg(uint8_t addr, uint16_t value)
{
  uint16_t dac_value = 0;
  uint8_t dac_cmd = 0;

  // pass the register value directly to DAC
  dac_value = value;
  dac_cmd = 0x0F & (addr);

  mcp_hv.digitalWrite(HV_DAC_CS_0, LOW);
  //mcp_bottom.digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  SPI.transfer(dac_cmd);
  SPI.transfer16(dac_value);
  SPI.endTransaction();
  //mcp_bottom.digitalWrite(DAC_CS, HIGH);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);
}

uint16_t dac_read_reg(uint8_t addr)
{

  uint32_t dac_value = 0;
  uint8_t dac_cmd = 0;

  dac_cmd = 0x80 | (addr & 0x0F);     // make sure the upper nibble is zero

  //Serial.print("DAC read command: ");
  //Serial.println(dac_cmd,HEX);

  // send the dac command and read back the value
  mcp_hv.digitalWrite(HV_DAC_CS_0, LOW);
  //mcp_bottom.digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  SPI.transfer(dac_cmd);
  SPI.transfer16(0x0000);
  dac_value = SPI.transfer32(0x00000000);
  SPI.endTransaction();
  //mcp_bottom.digitalWrite(DAC_CS, HIGH);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);

  return dac_value;

}

void dac_set_output(uint8_t channel, uint16_t value)
{
  // bit shift the data to align with MSB
  value <<= 4;
  //value &= 0xFFF0;

  mcp_hv.digitalWrite(HV_DAC_CS_0, LOW);
  //mcp_bottom.digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  SPI.transfer(HV_DAC_DAC_BASE_ADDR + channel);
  //SPI.transfer16(dac_value);
  SPI.transfer16(value);
  SPI.endTransaction();
  //mcp_bottom.digitalWrite(DAC_CS, HIGH);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);
}

void dac_clear_output(void)
{

  dac_set_output(0, 0x0000);
  dac_set_output(1, 0x0000);
  dac_set_output(2, 0x0000);
  dac_set_output(3, 0x0000);
  dac_set_output(4, 0x0000);
  dac_set_output(5, 0x0000);
  dac_set_output(6, 0x0000);
  dac_set_output(7, 0x0000);

}