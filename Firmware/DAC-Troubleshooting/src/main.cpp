#include <Arduino.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SPI.h>
#include <elapsedMillis.h>

// put function declarations here:

//-------------------------------------------------------------------------------------------
//                                       INCLUDES
//-------------------------------------------------------------------------------------------

#include "pcb-v1.h"
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
elapsedMillis loopTimeElapsed, runTimeElapsed = 0;
uint32_t loop_period      = 10; // ms   
uint32_t run_time         = 5000; // ms 

bool robot_enabled = true;
bool ramp_up_finished = false;
uint16_t ramp_length = 500; // ms
// NeoPixel Objects

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

  Serial.begin(9600);
  Serial.println("MCP23xxx Combo Test!");

  // initialize the MCP23017 pin expander on all boards
  mcp_bottom.begin_I2C(GPIO_BOTTOM_ADDR);
  mcp_top.begin_I2C(GPIO_TOP_ADDR);
  mcp_hv.begin_I2C(HV_GPIO_ADDR);

  // turn on the high voltage board 
  mcp_bottom.pinMode(BRD_ENABLE_1, OUTPUT);
  mcp_bottom.digitalWrite(BRD_ENABLE_1, HIGH);
  // enable the power monitor on the PCB V1
  mcp_bottom.pinMode(PWR_SHDN_L, OUTPUT);
  mcp_bottom.digitalWrite(PWR_SHDN_L, HIGH);

  // enable the HV boost converter charge pump
  //mcp_hv.pinMode(HV_CP_EN_0, OUTPUT);
  // make sure the HV boost converter is disabled
  //mcp_hv.pinMode(HV_CS_0, OUTPUT);
  //hv_disable();

  //
  mcp_bottom.pinMode(DAC_CS, OUTPUT);

  // set DAC CS to output
  mcp_hv.pinMode(HV_DAC_CS_0, OUTPUT);
  // clear output of all channels
  //dac_clear_all();
  //dac_write(HV_DAC_BRDCAST, 0x0FFF);  // update all output registers
  dac_write_reg(HV_DAC_SYNC, 0x0000);     // set SYNC to 0
  dac_write_reg(HV_DAC_CONFIG, 0x0000);   // no CRC, DO enabled, all DACs enabled
  dac_write_reg(HV_DAC_GAIN, 0x00FF);     // set gain to 1
  dac_write_reg(HV_DAC_TRIGGER, 0x0008);  // set trigger to 0
  dac_clear_output();

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


}

void loop() {

// run code here
  if(loopTimeElapsed > loop_period)
  {
    Serial.println('running leg controller now');
    Serial.print(leg_1_lift);
    Serial.print(" ");
    Serial.print(leg_1_swing);
    // reset loop timer
    loopTimeElapsed = 0;

    if(robot_enabled)
    {
      // set flag that we are ramped up
      if(!ramp_up_finished)
      {
        leg_1.startRamp(true, ramp_length);
        leg_2.startRamp(true, ramp_length);
        leg_3.startRamp(true, ramp_length);
        leg_4.startRamp(true, ramp_length);

        // ramp up the voltages here
        while(!ramp_up_finished)
        {
          leg_1.run(&leg_1_lift, &leg_1_swing, true);
          leg_2.run(&leg_2_lift, &leg_2_swing, true);
          leg_3.run(&leg_3_lift, &leg_3_swing, true);
          leg_4.run(&leg_4_lift, &leg_4_swing, true);
        }
        // 
        ramp_up_finished = true;
      }
      else
      {
        //execute waveform and RICK reading code here!
        leg_1.run(&leg_1_lift, &leg_1_swing, true);
        leg_2.run(&leg_2_lift, &leg_2_swing, true);
        leg_3.run(&leg_3_lift, &leg_3_swing, true);
        leg_4.run(&leg_4_lift, &leg_4_swing, true);
      }
    }
    else
    {
      // disable the high voltage converter
      hv_disable();
      // make sure the ramp flag is reset for next time
      ramp_up_finished = false;
    }

  }


}

void hv_enable(void)
{
  mcp_hv.digitalWrite(HV_CP_EN_0, HIGH);

  mcp_hv.digitalWrite(HV_CS_0, LOW);
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  SPI.transfer16(0x00F4);
  //SPI.transfer16(0x00E4);
  SPI.endTransaction();
  mcp_hv.digitalWrite(HV_CS_0, HIGH);

}

void hv_disable(void)
{
  // disable the charge pump
  mcp_hv.digitalWrite(HV_CP_EN_0, LOW);

  // enable the DC-DC converter
  mcp_hv.digitalWrite(HV_CS_0, LOW);
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  //SPI.transfer16(0x00F4);
  //SPI.transfer16(0x00E4);
  SPI.transfer16(0x00FA); // shutdown converter
  SPI.endTransaction();
  mcp_hv.digitalWrite(HV_CS_0, HIGH);

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

  Serial.print("DAC write command: ");
  Serial.println(dac_cmd,HEX);

  mcp_hv.digitalWrite(HV_DAC_CS_0, LOW);
  //mcp_bottom.digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
  SPI.transfer(dac_cmd);
  SPI.transfer16(dac_value);
  SPI.endTransaction();
  //mcp_bottom.digitalWrite(DAC_CS, HIGH);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);
}

uint16_t dac_read_reg(uint8_t addr)
{

  uint16_t dac_value = 0;
  uint8_t dac_cmd = 0;

  dac_cmd = 0x80 | (addr & 0x0F);     // make sure the upper nibble is zero

  Serial.print("DAC read command: ");
  Serial.println(dac_cmd,HEX);

  // send the dac command and read back the value
  mcp_hv.digitalWrite(HV_DAC_CS_0, LOW);
  //mcp_bottom.digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
  SPI.transfer(dac_cmd);
  dac_value = SPI.transfer16(0x0000);
  SPI.endTransaction();
  //mcp_bottom.digitalWrite(DAC_CS, HIGH);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);

  return dac_value;

}

void dac_set_output(uint8_t channel, uint16_t value)
{
  uint16_t dac_value = 0x0FFF & value;

  mcp_hv.digitalWrite(HV_DAC_CS_0, LOW);
  //mcp_bottom.digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
  SPI.transfer(channel);
  //SPI.transfer16(dac_value);
  SPI.transfer16(dac_value);
  SPI.endTransaction();
  //mcp_bottom.digitalWrite(DAC_CS, HIGH);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);
}

void dac_clear_output(void);
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
