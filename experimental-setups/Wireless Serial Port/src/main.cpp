#include <Arduino.h>

// put function declarations here:
char data;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  // if we received data over the USB serial port, send it out the hardware serial port
  if(Serial.available()){

    // read the data and echo it back to the serial monitor:
    data = Serial.read();
    Serial.print(data);

    digitalWrite(LED_BUILTIN, HIGH);
    Serial1.write(data);
    digitalWrite(LED_BUILTIN, LOW);
  }

  // likely need to format the data for the printer.
  // if we received data over the hardware serial port, print it out the USB serial monitor
  if (Serial1.available()){
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.write(Serial1.read());
    digitalWrite(LED_BUILTIN, LOW);
  }
  {
    /* code */
  }
  
}

