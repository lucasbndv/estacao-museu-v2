#include "include/config.c"
#include "include/database.c"
#include "include/webserver.c"

// Time from reset to 'database_save_data()' is aprox. 750ms
//
#define TIME_TO_SLEEP 9250000 /* Time ESP32 will go to sleep (in us) */

void setup()
{

  Serial.begin(115200);

  //if (config("/config/config.cfg"))
  //{
  //  config_set_default();
  //}

  analogSetSamples(4);
  analogSetClockDiv(8);

  database_setup();
  database_save_data();

  //webserver_setup();
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
  esp_deep_sleep_start();
  
}

void loop()
{
}
