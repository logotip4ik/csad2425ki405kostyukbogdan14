#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <htmx.h>

#include "document.h"

typedef struct {
  int value;
} State;

const char* ssid = "ESP32";
const char* password = "12341234";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);
State state = { .value = 0 };

void sendNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendHtmx(AsyncWebServerRequest *request) {
  // htmx is a bit to large to send in one chunk, so we need to split it and send in chunks
  AsyncWebServerResponse* response = request->beginChunkedResponse("text/javascript", [](uint8_t* buffer, size_t maxLen, size_t index) {
    int toCopy = min(htmxSize - index, maxLen);
    if (toCopy == 0) {
      return 0;
    }

    memcpy(buffer, htmx + index, toCopy);

    return toCopy;
  });

  request->send(response);
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);

  server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    state.value = 0;

    request->send(200, "text/html", document);
  });

  server.on("/counter-up", HTTP_POST, [&](AsyncWebServerRequest *request){
    String counterString("counter: ");
    counterString.concat(state.value += 1);

    request->send(200, "text/html", counterString);
  });

  server.on("/counter-down", HTTP_POST, [&](AsyncWebServerRequest *request){
    String counterString("counter: ");
    counterString.concat(state.value -= 1);

    request->send(200, "text/html", counterString);
  });

  server.on("/counter-reset", HTTP_POST, [&](AsyncWebServerRequest *request){
    String counterString("counter: ");
    counterString.concat(state.value = 0);

    request->send(200, "text/html", counterString);
  });

  server.on("/htmx.js", HTTP_GET, sendHtmx);
  server.onNotFound(sendNotFound);

  server.begin();

  Serial.println("HTTP server started");
}

void loop() {
}
