/*
  WinsenZE06.cpp - This library allows you to set and read the ZE06 Winsen Sensor module.
  Created by Igor Kelvin, 2020.
  Based on WinsenZE03 by Fabian Gutierrez
  MIT.
*/

#include "Arduino.h"
#include "WinsenZE06.h"

WinsenZE06::WinsenZE06(){
  _s  = NULL;
}

void WinsenZE06::begin(Stream *ser){
  _s = ser;
  
  _PM10    = -1;
  _PM2dot5 = -1;
  _PM1dot0 = -1;
  
  setMode(QA);
}

void WinsenZE06::setMode(ZE06Mode mode){
  uint8_t setConfig[9] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46}; //QA config
  uint8_t response[9]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  _s->write(setConfig, sizeof(setConfig));
}

uint8_t WinsenZE06::update() {
  uint8_t petition[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  uint8_t measure[9]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  
  _PM10    = -1;
  _PM2dot5 = -1;
  _PM1dot0 = -1;
  
  //flush
  while (_s->available() > 0) {
    uint8_t c = _s->read();
  }
  
  _s->write(petition, sizeof(petition));
  
  // read
  size_t bytes = _s->readBytes(measure, 9);
  
  if (bytes != 9) {
    return 1;
  }
  
#ifdef ZE06_DEBUG
  debugPrint(measure);
  Serial.printf("Checksum: %x\n", checkSum(measure));
#endif

  // calculate
  if (measure[0] == 0xFF && measure[1] == 0x86) {
    if (checkSum(measure) == measure[8]) {
      _PM2dot5 = (measure[2] << 8) + measure[3];
      _PM10    = (measure[4] << 8) + measure[5];
      _PM1dot0 = (measure[6] << 8) + measure[7];
      
      return 0;
    }
  } 
  
  return 1;
}



uint8_t WinsenZE06::getPM10() {
  return _PM10;
}
uint8_t WinsenZE06::getPM2dot5() {
  return _PM2dot5;
}

uint8_t WinsenZE06::getPM1dot0() {
  return _PM1dot0;
}

uint8_t WinsenZE06::checkSum(uint8_t *measure_byte) {
  uint8_t tmp = 0;
  uint8_t i;
  
  for (i = 1; i < 8; i++) {
    tmp += measure_byte[i];
  }
  
  return (~tmp) + 1;
}

#ifdef ZE06_DEBUG
void WinsenZE06::debugPrint(byte *measure) {
  uint8_t i;
  
  for (i = 0; i < 9; i++) {
    Serial.printf("%02X ", measure[i]);
  }
  Serial.println("");
}
#endif
