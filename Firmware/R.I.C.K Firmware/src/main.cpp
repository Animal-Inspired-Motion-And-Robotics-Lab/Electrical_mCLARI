#include <Arduino.h>


// libraries

#include "MCP23008.h"
#include "Arduino_ST7789.h"

MCP23008 MCP(0x24);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  Wire.begin();
  MCP.begin();

  Serial.print("MCP23008_test version: ");
  Serial.println(MCP23008_LIB_VERSION);

  Serial.print("Connect: ");
  Serial.println(MCP.isConnected());


  for (int pin = 0; pin < 8; pin++)
  {
    MCP.pinMode(pin, INPUT);
  }
  Serial.println("TEST digitalRead(pin)");
}

void loop() {

  for (int pin = 0; pin < 8; pin++)
  {
    int val = MCP.digitalRead(pin);
    Serial.print(val);
    Serial.print('\t');
  }
  Serial.println();
  delay(100);

}
