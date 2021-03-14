#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

void BMP280Setup()
{
  if (!bme.begin(0x76))
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1)
      ;
  }
}

float BMP280ReadTemperature()
{
  return bme.readTemperature();
}

float BMP280ReadPressure()
{
  return bme.readPressure();
}

float BMP280ReadAltitude()
{
  return bme.readAltitude(1013.25);
}

float BMP280ReadHumidity()
{
  return bme.readHumidity();
}
