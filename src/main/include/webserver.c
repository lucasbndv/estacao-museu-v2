#include <WiFiClient.h>
#include <ESPmDNS.h>

#include <sqlite3.h>
#include <Arduino_JSON.h>

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "JSONHandler.h"

JSONHandler *json_handler;

AsyncWebServer server(80);

void webserver_cb_notfound(AsyncWebServerRequest *request)
{
  request->send(SPIFFS, "/index.html", "text/html");
}

void webserver_cb_root(AsyncWebServerRequest *request)
{
  request->send(200, "oi", "text/plain");
}

void webserver_cb_api_config(AsyncWebServerRequest *request)
{
  Serial.println("[WEB] In request '/api/config'");

  JSONVar data = *((JSONVar *)(request->_tempObject));

  Serial.println(data);

  JSONVar keys = data.keys();

  if (!data.hasOwnProperty("ssid") || !data.hasOwnProperty("pass"))
  {
    Serial.println("Invalid ap-name/ap-pass combination");
    request->send(400, "text/plain", "Invalid ap-name/ap-pass combination");
    return;
  }

  if (data["ssid"] == _cfg["wifi"]["ssid"])
  {
    Serial.printf("[WEB] Already connected in this ssid\n");
    request->send(400, "text/plain", "Already connected");
    return;
  }

  Serial.println();
  request->send(200);

  _cfg["wifi"]["ssid"] = data["ssid"];
  _cfg["wifi"]["pass"] = data["pass"];

  config_save();
  config_execute();
}

void webserver_cb_dashboard(AsyncWebServerRequest *request)
{
  Serial.println("[WEB] In request '/api/filters'");
  JSONVar data = *((JSONVar *)(request->_tempObject));

  char sql[512];
  snprintf(sql, sizeof(sql), "SELECT * FROM sensordata WHERE datetime LIKE ´%s´",
           (const char *)data["date_from"]);
  Serial.println(sql);
  JSONVar data_resp = db.query(sql);

  request->send(200, "aplication/json", JSON.stringify(data_resp));
}

void webserver_setup()
{
  uint64_t chipid = ESP.getEfuseMac();
  char str[32] = "";
  snprintf(str, sizeof(str), "estacao-cefet-%04x", (uint16_t)(chipid >> 32));

  if (MDNS.begin(str))
  {
    Serial.printf("[WEB] MDNS responder started @ http://%s.local\n", str);
    MDNS.addService("http", "tcp", 80);
  }

  server.serveStatic("/", SPIFFS, "/website/").setDefaultFile("index.html");
  ;

  JSONHandler *json_config = new JSONHandler();

  json_config->setUri("/api/config");
  json_config->setMethod(HTTP_POST);
  json_config->onRequest(webserver_cb_api_config);
  //server.on("/", webserver_cb_root);

  json_config->setUri("/api/filters");
  json_config->setMethod(HTTP_POST);
  json_config->onRequest(webserver_cb_dashboard);

  server.addHandler(json_config);
  server.onNotFound(webserver_cb_root);
  server.begin();

  Serial.println("[WEB] HTTP server started");
}
