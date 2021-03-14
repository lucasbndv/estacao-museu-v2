/*
  WinsenZE03.h - This library allows you to set and read the ZE03 Winsen Sensor module.
	More information: https://github.com/fega/winsen-ze03-arduino-library
  Created by Fabian Gutierrez <fega.hg@gmail.com>, March 17, 2017.
  MIT.
*/
#include <SoftwareSerial.h>
#include <WinsenZE06.h>

SoftwareSerial SSerial(16, 17); //RX TX
WinsenZE06 ZH06;


void setup() {
  Serial.begin(115200);

  SSerial.begin(9600);
  
  ZH06.begin(&SSerial);
}
void loop() {
  uint8_t err = ZH06.update();
  
  if (err) {
    Serial.println("failed");
    return;
  }
  
  Serial.print(ZH06.getPM1dot0());
  Serial.print(", ");
  Serial.print(ZH06.getPM2dot5());
  Serial.print(", ");
  Serial.print(ZH06.getPM10());
  Serial.println("");

  delay(1000);
}
