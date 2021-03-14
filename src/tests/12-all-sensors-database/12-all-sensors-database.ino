#include <FS.h>
#include <SPI.h>
#include "SD.h"

#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <HardwareSerial.h>
#include <WinsenZE06.h>
#include "kSeries.h"

RTC_DS3231 rtc;


#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;

#define PIN_LDR 4

kSeries S8(13, 26); //RX TX

HardwareSerial HSerial(1);
WinsenZE06 ZH06;

#include <JSONDatabase.h>

JSONDatabase db;

void setup()
{
  Serial.begin(115200);

  if (!SD.begin()) {
    Serial.println("SD Mount Failed");
    while (1);
  }

  db.open("/sd/data/estacao3.db");

  JSONVar data;

  data["datetime"]    = "DATETIME"; /* RTC */
  data["temperature"] = "FLOAT";    /* BME280 */
  data["pressure"]    = "FLOAT";    /* BME280 */
  data["humidity"]    = "FLOAT";    /* BME280 */
  data["luminosity"]  = "FLOAT";    /* LDR */
  data["CO2"]         = "FLOAT";    /* SenseAir S8 */
  data["PM1.0"]       = "FLOAT";    /* WinsenZE06 */
  data["PM2.5"]       = "FLOAT";    /* WinsenZE06 */
  data["PM10"]        = "FLOAT";    /* WinsenZE06 */

  db.create_table("sensordata", &data);

  //RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));

  //BME280
  int status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    while (1);
  }

  //LDR
  pinMode(PIN_LDR, INPUT);

  //SenseAir S8
  // (no config)

  //ZH06
  HSerial.begin(9600, SERIAL_8N1, 16, 17);
  ZH06.begin(&HSerial);
}

void loop()
{
  JSONVar data;

  DateTime now = rtc.now();
  char datetime[64] = "%04d-%02d-%02d %02d:%02d:%02d";
  snprintf(datetime, sizeof(datetime), datetime,
           now.year(),
           now.month(),
           now.day(),
           now.hour(),
           now.minute(),
           now.second());

  uint8_t err = ZH06.update();

  if (err) {
    Serial.println("ZH06 failed");
  }

  data["datetime"]    = datetime;
  data["temperature"] = bme.readTemperature();       /* °C */
  data["pressure"]    = bme.readPressure() / 100.0F; /* hPa */
  data["humidity"]    = bme.readHumidity();          /* % */
  data["luminosity"]  = analogRead(PIN_LDR);
  data["CO2"]         = S8.getCO2('p');
  data["PM1.0"]       = ZH06.getPM1dot0();
  data["PM2.5"]       = ZH06.getPM2dot5();
  data["PM10"]        = ZH06.getPM10();

  db.insert("sensordata", &data);
  
  delay(5000);

}
