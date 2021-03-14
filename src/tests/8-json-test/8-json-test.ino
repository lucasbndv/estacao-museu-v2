//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino_JSON.h>

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "include/JSONHandler.h"

JSONHandler* json_handler;

AsyncWebServer server(80);

const char* ssid = "LIVE TIM_AWAR_2G";
const char* password = "5Ea668fC62";

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
    while (1) {
    }
  }
}

void onRootRequest(AsyncWebServerRequest *request) {
  request->send(SPIFFS, "/wifi.html", "text/html");
}

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  initSPIFFS();

  server.on("/", onRootRequest);

  server.on("/api/ap-scan", HTTP_GET, [](AsyncWebServerRequest * request) {

    JSONVar response;

    int n = WiFi.scanNetworks();

    for (int i = 0; i < n; ++i) {
      response[i]["SSID"] = WiFi.SSID(i);
      response[i]["RSSI"] = WiFi.RSSI(i);
    }

    Serial.printf("Response: %s", JSON.stringify(response).c_str());
    request->send(200, "application/json", JSON.stringify(response));

  });


  json_handler = new JSONHandler();

  json_handler->setUri("/api/config");
  json_handler->setMethod(HTTP_POST);
  json_handler->onRequest([](AsyncWebServerRequest *request) {
    Serial.println("in request");

    JSONVar json = *((JSONVar *)(request->_tempObject));
    
    Serial.println(json);

    JSONVar keys = json.keys();

    if (!json.hasOwnProperty("ap-name") || !json.hasOwnProperty("ap-pass")) {
      Serial.println("Invalid ap-name/ap-pass combination");
      request->send(400, "text/plain", "Invalid ap-name/ap-pass combination");
    }

    Serial.println();
    request->send(200);
    
    WiFi.disconnect();
    WiFi.begin(json["ap-name"], json["ap-pass"]);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }

    Serial.println(WiFi.SSID());
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

  });

  server.addHandler(json_handler);

  server.begin();
}


void loop() {
}
