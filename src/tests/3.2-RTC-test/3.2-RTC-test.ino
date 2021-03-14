#include <Wire.h>
#include "RTClib.h"

RTC_DS3231 rtc;

void setup()
{

  Serial.begin(115200);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  rtc.adjust(DateTime(__DATE__, __TIME__) + TimeSpan(5));
}

void loop()
{
  DateTime now = rtc.now();

  //Serial.println(now.timestamp()) /* I don't like this output */
  
  Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n", now.year(), 
                                                   now.month(), 
                                                   now.day(), 
                                                   now.hour(), 
                                                   now.minute(), 
                                                   now.second());

  delay(1000);
}
