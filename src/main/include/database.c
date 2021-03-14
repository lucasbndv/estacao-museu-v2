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

#define PIN_LDR 34
#define PIN_ERROR 33

kSeries S8(13, 26); //RX TX

HardwareSerial HSerial(1);
WinsenZE06 ZH06;

#include <JSONDatabase.h>

JSONDatabase db;

void blink_pin(int pin, unsigned int n = 1, bool invert = true, int _delay = 200)
{
  pinMode(pin, OUTPUT);
  int state = digitalRead(pin);
  for (int i = 0; i < n; i++)
  {
    digitalWrite(pin, !state ^ invert);
    delay(_delay);
    digitalWrite(pin, state ^ invert);
    delay(_delay);
  }

  //todo: restore initial pinMode
}

void database_setup()
{

  if (!SD.begin())
  {
    Serial.println("[DB] SD Mount Failed");
    blink_pin(PIN_ERROR, 1);
    ESP.restart();
  }

  //RTC
  // if (!rtc.begin())
  // {
  //   Serial.println("[DB] Couldn't find RTC");
  //   digitalWrite(PIN_ERROR, LOW);
  //   blink_pin(PIN_ERROR, 2);
  //   ESP.restart();
  // }

  // We won't be adjusting time on main sketch anymore,
  // RTC time will be ajusted on test phase.

  //rtc.adjust(DateTime(__DATE__, __TIME__));

  //BME280
  int status = bme.begin(0x76);
  if (!status)
  {
    Serial.println("[DB] Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    blink_pin(PIN_ERROR, 3);
    ESP.restart();
  }

  //LDR
  //pinMode(PIN_LDR, INPUT);

  //SenseAir S8
  // (no config)

  //ZH06
  // HSerial.begin(9600, SERIAL_8N1, 16, 17);
  // ZH06.begin(&HSerial);

  db.open("/sd/data/estacao20.db");

  JSONVar data;

  data["datetime"] = "DATETIME"; /* RTC */
  data["temperature"] = "FLOAT"; /* BME280 */
  data["pressure"] = "FLOAT";    /* BME280 */
  data["humidity"] = "FLOAT";    /* BME280 */
  data["luminosity"] = "FLOAT";  /* LDR */
  data["CO2"] = "FLOAT";         /* SenseAir S8 */
  //data["PM1.0"] = "FLOAT";        /* WinsenZE06 */
  //data["PM2.5"] = "FLOAT";        /* WinsenZE06 */
  //data["PM10"] = "FLOAT";       /* WinsenZE06 */
  data["dust10"] = "FLOAT";  /* teste */
  data["dust25"] = "FLOAT";  /* teste */
  data["dust100"] = "FLOAT"; /* teste */

  db.create_table("sensordata", &data);
}

int database_save_data()
{
  JSONVar data;

  // DateTime now = rtc.now();
  // char datetime[64] = "%04d-%02d-%02d %02d:%02d:%02d";
  // snprintf(datetime, sizeof(datetime), datetime,
  //          now.year(),
  //          now.month(),
  //          now.day(),
  //          now.hour(),
  //          now.minute(),
  //          now.second());

  // uint8_t err = ZH06.update();

  // if (err)
  // {
  //   blink_pin(PIN_ERROR, 4);
  //   Serial.println("ZH06 failed");
  // }

  //data["datetime"] = datetime;
  data["temperature"] = bme.readTemperature();    /* °C */
  data["pressure"] = bme.readPressure() / 100.0F; /* hPa */
  data["humidity"] = bme.readHumidity();          /* % */
  //data["luminosity"] = analogRead(PIN_LDR);
  data["luminosity"] = 300; //TESTE
  //data["CO2"] = S8.getCO2('p');
  //data["PM1.0"] = ZH06.getPM1dot0();
  data["dust10"] = 1; //TESTE
  //data["PM2.5"] = ZH06.getPM2dot5();
  data["dust25"] = 2.5; //TESTE
  //data["PM10"] = ZH06.getPM10();
  data["dust100"] = 10; //TESTE

  Serial.println("[INSERT]");
  Serial.println(data);
  db.insert("sensordata", &data);

  // checking program answer
  bool loop = true;
  String ans = "";
  int timer = millis();
  while (loop)
  {
    while (Serial.available())
    {
      ans += Serial.read();
    }

    if (millis() - timer > 1000)
    {
      loop = false;
    }
  }
  //db.sendBackup();
}
