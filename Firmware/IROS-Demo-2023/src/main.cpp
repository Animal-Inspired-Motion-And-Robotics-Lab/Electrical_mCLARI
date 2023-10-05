#include <Arduino.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SPI.h>

// put function declarations here:

//-------------------------------------------------------------------------------------------
//                                       INCLUDES
//-------------------------------------------------------------------------------------------

#include "pcb-v1.h"

// led functions
uint32_t Wheel(byte WheelPos);
void rainbowCycle(uint8_t wait);

// hv dac functions
void dac_write(uint8_t channel, uint16_t value);
uint16_t dac_read(uint8_t addr);

void dac_clear_all(void);
void dac_write_all(uint16_t value);
void dac_print(void);

// hv functions 
void hv_disable(void);
void hv_enable(void);

// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

Adafruit_MCP23X17 mcp_top;
Adafruit_MCP23X17 mcp_bottom;
Adafruit_MCP23X17 mcp_hv;

#define WIRE Wire

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN MCU_NEOPIXEL  

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  55

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
// Fill the dots one after the other with a color

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

  // initialize the mcp23017
  for(int i=0;i<LED_COUNT;i++)
  {
        strip.setPixelColor(i,255,255,96);
  }

  strip.show(); // Initialize all pixels to 'off'


}

void loop() {

  // //rainbow(20);

}

void hv_enable(void)
{
  mcp_hv.digitalWrite(HV_CP_EN_0, HIGH);

  mcp_hv.digitalWrite(HV_CS_0, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
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
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
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
  Serial.println(dac_read(0x00));
  Serial.print("Sync: ");
  Serial.println(dac_read(HV_DAC_SYNC));
  Serial.print("Gain: ");
  Serial.println(dac_read(HV_DAC_GAIN));
  Serial.print("Trigger: ");
  Serial.println(dac_read(HV_DAC_TRIGGER));
  Serial.print("Broadcast: ");
  Serial.println(dac_read(HV_DAC_BRDCAST));
  Serial.print("Status: ");
  Serial.println(dac_read(HV_DAC_STATUS));
}

void dac_write(uint8_t addr, uint16_t value)
{
  uint16_t dac_value = 0;
  uint8_t dac_cmd = 0;

  dac_value = value & 0x0FFF;
  dac_cmd = 0x0F & (addr);

  Serial.print("DAC write command: ");
  Serial.println(dac_cmd,HEX);

  mcp_hv.digitalWrite(HV_DAC_CS_0, LOW);
  //mcp_bottom.digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  SPI.transfer(dac_cmd);
  SPI.transfer16(dac_value);
  SPI.endTransaction();
  //mcp_bottom.digitalWrite(DAC_CS, HIGH);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);
}

uint16_t dac_read(uint8_t addr)
{

  uint16_t dac_value = 0;
  uint8_t dac_cmd = 0;

  dac_cmd = 0x80 | (addr & 0x0F);     // make sure the upper nibble is zero

  Serial.print("DAC read command: ");
  Serial.println(dac_cmd,HEX);

  // send the dac command and read back the value
  mcp_hv.digitalWrite(HV_DAC_CS_0, LOW);
  //mcp_bottom.digitalWrite(DAC_CS, LOW);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  SPI.transfer(dac_cmd);
  dac_value = SPI.transfer16(0x0000);
  SPI.endTransaction();
  //mcp_bottom.digitalWrite(DAC_CS, HIGH);
  mcp_hv.digitalWrite(HV_DAC_CS_0, HIGH);

  return dac_value;

}

void dac_clear_all(void)
{
  dac_write(HV_DAC_DAC0, 0);
  dac_write(HV_DAC_DAC1, 0);
  dac_write(HV_DAC_DAC2, 0);
  dac_write(HV_DAC_DAC3, 0);
  dac_write(HV_DAC_DAC4, 0);
  dac_write(HV_DAC_DAC5, 0);
  dac_write(HV_DAC_DAC6, 0);
  dac_write(HV_DAC_DAC7, 0);
}

void dac_write_all(uint16_t value)
{
  dac_write(HV_DAC_DAC0, value);
  dac_write(HV_DAC_DAC1, value);
  dac_write(HV_DAC_DAC2, value);
  dac_write(HV_DAC_DAC3, value);
  dac_write(HV_DAC_DAC4, value);
  dac_write(HV_DAC_DAC5, value);
  dac_write(HV_DAC_DAC6, value);
  dac_write(HV_DAC_DAC7, value);
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}