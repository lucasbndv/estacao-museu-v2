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
#define DEVICE "museu1"
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
  if (!rtc.begin())
  {
    Serial.println("[DB] Couldn't find RTC");
    digitalWrite(PIN_ERROR, LOW);
    blink_pin(PIN_ERROR, 2);
    ESP.restart();
  }

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
  pinMode(PIN_LDR, INPUT);

  //SenseAir S8
  // (no config)

  //ZH06
  HSerial.begin(9600, SERIAL_8N1, 16, 17);
  ZH06.begin(&HSerial);

  db.open("/sd/data/estacao30.db");

  JSONVar data;
  data["device"] = "VARCHAR(255)";                  /* Device id */
  data["id"] = "INTEGER PRIMARY KEY AUTOINCREMENT"; /* Data id */
  data["datetime"] = "DATETIME";                    /* RTC */
  data["temperature"] = "FLOAT";                    /* BME280 */
  data["humidity"] = "FLOAT";                       /* BME280 */
  data["dewpoint"] = "FLOAT";                       /* Calc */
  data["absolutehumidity"] = "FLOAT";               /* Calc */
  data["pressure"] = "FLOAT";                       /* BME280 */
  data["luminosity"] = "FLOAT";                     /* LDR */
  data["co2"] = "FLOAT";                            /* SenseAir S8 */
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

  if (err)
  {
    blink_pin(PIN_ERROR, 4);
    Serial.println("ZH06 failed");
  }

  float temp = bme.readTemperature();
  float humi = bme.readHumidity();
  data["device"] = DEVICE;
  data["datetime"] = datetime;
  data["temperature"] = temp;                                                                                    /* °C */
  data["humidity"] = humi;                                                                                       /* % */
  data["dewpoint"] = pow((humi / 100), (1.0 / 8.0)) * (112 + 0.9 * temp) + (0.1 * temp - 112);                   /* °C */
  data["absolutehumidity"] = (pow(2.71828, (temp * 17.67) / (temp + 243.5)) * humi * 13.2471) / (273.15 + temp); /* g/m3 */
  data["pressure"] = bme.readPressure() / 100.0F;                                                                /* hPa */
  data["luminosity"] = analogRead(PIN_LDR);
  //data["luminosity"] = 300; //TESTE
  data["co2"] = S8.getCO2('p');
  //data["PM1.0"] = ZH06.getPM1dot0();
  data["dust10"] = ZH06.getPM1dot0(); //TESTE
  //data["PM2.5"] = ZH06.getPM2dot5();
  data["dust25"] = ZH06.getPM2dot5(); //TESTE
  //data["PM10"] = ZH06.getPM10();
  data["dust100"] = ZH06.getPM10(); //TESTE

  db.insert("sensordata", &data);
  JSONVar ans = db.query("SELECT id FROM sensordata ORDER BY id DESC LIMIT 1")[0];
  int id = atol((const char *)ans["id"]);
  data["dataId"] = id;
  //data["dataId"] = ans["id"].toInt();
  Serial.println("[INSERT]");
  Serial.println(data);

  int wait_response = millis();
  long int response = 0;
  while (!Serial.available() && millis() - wait_response < 1000)
  {
    Serial.println("Starting backup...");
    response = Serial.parseInt();
  }

  if (response)
  {
    db.sendBackup(response);
    Serial.println("DB REQUESTED");
  }
}
