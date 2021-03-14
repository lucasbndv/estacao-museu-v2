/*
  WinsenZE06.h - This library allows you to set and read the ZE06 Winsen Sensor module.
  Created by Igor Kelvin, 2020.
  Based on WinsenZE03 by Fabian Gutierrez
  MIT.
*/

#ifndef WinsenZE06_h
#define WinsenZE06_h

#if defined(ARRAY_SIZE)
#else
 #define ARRAY_SIZE(x) ((sizeof(x))/(sizeof(*(x))))
#endif

#include "Arduino.h"
#include <stdint.h>

//#define ZE06_DEBUG

enum ZE06Mode {
  QA, /* Question & Answer */
  IU  /* Initiative Upload */
};

class WinsenZE06 {
  public:
    WinsenZE06();
    void    begin(Stream *ser);
    void    setMode(ZE06Mode mode);
    uint8_t update();
    
    uint8_t getPM10();
    uint8_t getPM2dot5();
    uint8_t getPM1dot0();
  private:
    int16_t _PM10;
    int16_t _PM2dot5;
    int16_t _PM1dot0;
    Stream *_s;

    uint8_t checkSum(uint8_t *measure_type);
    
#ifdef ZE06_DEBUG        
    void debugPrint(byte arr[]);
#endif

};

#endif /* WinsenZE06_h */
