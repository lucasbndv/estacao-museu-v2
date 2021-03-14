/*
  WinsenZE03.h - This library allows you to set and read the ZE03 Winsen Sensor module.
	More information: https://github.com/fega/winsen-ze03-arduino-library
  Created by Fabian Gutierrez <fega.hg@gmail.com>, March 17, 2017.
  MIT.
*/
#include <HardwareSerial.h>
#include <WinsenZE06.h>

HardwareSerial HSerial(1);
WinsenZE06 ZH06;


void setup() {
  Serial.begin(115200);

  HSerial.begin(9600, SERIAL_8N1, 16, 17);
  ZH06.begin(&HSerial);
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
