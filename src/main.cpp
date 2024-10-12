#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <htmx.h>

#include "document.h"

const char* ssid = "ESP32";
const char* password = "12341234";

typedef struct {
  int value;
} State;

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);
State state = { .value = 0 };

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void sendHtmx(AsyncWebServerRequest *request) {
    request->send(200, "text/javascript", htmx);
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
  server.onNotFound(notFound);

  server.begin();

  Serial.println("Starting http server");
}

void loop() {
}
