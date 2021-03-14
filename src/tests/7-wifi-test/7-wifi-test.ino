//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

AsyncWebServer server(80);

//const char* ssid = "LIVE TIM_AWAR_2G";
//const char* password = "5Ea668fC62";
const char* ssid = "DNAModas";
const char* password = "mengao81";

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
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  initSPIFFS();

  server.on("/", onRootRequest);
  //server.serveStatic("/", SPIFFS, "/");

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) {

    size_t argc = request->args();
    Serial.printf("args: %d\n", argc);

    for (int i = 0; i < argc; i++) {
      Serial.print(request->argName(i));
      Serial.print(": ");
      Serial.println(request->arg(i));
    }

    size_t headerc = request->headers();
    Serial.printf("headerc: %d\n", headerc);

    for (int i = 0; i < headerc; i++) {
      AsyncWebHeader *header = request->getHeader(i);
      Serial.print(header->toString());
    }

    AsyncClient *client = request->client();

    Serial.println(client->getRemoteAddress());
    Serial.println(client->getRemotePort());
    Serial.println(client->getLocalAddress());
    Serial.println(client->getLocalPort());

    request->send(200, "text/plain", "OK");
  });

  server.on("/api/ap-scan", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncResponseStream *response = request->beginResponseStream("text/plain");

    int n = WiFi.scanNetworks();
    response->print("[");
    for (int i = 0; i < n; ++i) {
      if (i != 0) {
        response->print(", ");
      }
      response->print("{\"ssid\": \"" + WiFi.SSID(i) + "\", \"rssi\": " + WiFi.RSSI(i) + "}");
    }
    response->print("]");
    
    request->send(response);

  });

  server.begin();
}

void loop() {
}
