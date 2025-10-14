/*
Calibration script for Boelgene v1.
*/


#include <SPI.h>
#include <MCP3XXX.h>


MCP3XXX_<10, 8, 300000> adc;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  SPI.setRX(16);
  SPI.setTX(19);
  SPI.setSCK(18);
  adc.begin(17);
}

void loop() {
  Serial.print(analogRead(26));
  Serial.print(" ");
  Serial.print(adc.analogRead(0));
  Serial.print(" ");
  Serial.print(adc.analogRead(1));
  Serial.print(" ");
  Serial.println(adc.analogRead(2));
  Serial.print("   ");
  Serial.println(analogRead(27));
  delay(100);
}
