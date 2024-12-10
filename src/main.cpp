#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <htmx.h>

#include "hunks.h"
#include "game.h"
#include "env.h"

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);
State state;

void sendHunk(AsyncWebServerRequest *request, String *hunk) {
  request->send(200, "text/html", hunk->begin());
}

void makeGameMove(AsyncWebServerRequest *request, State *state, Move move) {
  switch (state->mode) {
    case One: {
      state->player1Move = move;
      state->player2Move = makeAIMove(state);

      Result result = determineWinner(state->player1Move, state->player2Move);

      String hunk = String(oneAIHunkResults);
      hunk.replace("__PLAYER_MOVE__", getMoveString(state->player1Move));
      hunk.replace("__AI_MOVE__", getMoveString(state->player2Move));
      hunk.replace(
        "__RESULT__",
        result == Tie ? "It's a tie" : result == P1 ? "Player 1 wins!" : "AI wins!"
      );

      sendHunk(request, &hunk);

      break;
    }

    case Two: {
      if (state->player1Move == None) {
        state->player1Move = move;

        String hunk = String(makeMoveHunk);
        hunk.replace("__PLAYER_NAME__", "Player 2");

        sendHunk(request, &hunk);
      } else {
        state->player2Move = move;

        const Result result = determineWinner(state->player1Move, state->player2Move);

        String hunk = String(resultsHunk);
        hunk.replace(
          "__RESULT__",
          result == Tie ? "It's a tie" : result == P1 ? "Player 1 wins!" : "Player 2 wins!"
        );

        sendHunk(request, &hunk);
      }
      break;
    }
  }
}

void sendNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendHtmx(AsyncWebServerRequest *request) {
  // htmx is a bit too large to send in one chunk, so we need to split it and send in smaller chunks
  AsyncWebServerResponse* response = request->beginChunkedResponse("text/javascript", [](uint8_t* buffer, size_t maxLen, size_t index) -> size_t {
    int toCopy = min(htmxSize - index, maxLen);
    if (toCopy == 0) {
      return 0;
    }

    memcpy(buffer, htmx + index, toCopy);

    return toCopy;
  });

  request->send(response);
}

void connectToWifi(const char *ssid, const char *password) {
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.print("\nLocal ESP32 IP: ");
    Serial.println(WiFi.localIP());
}

const char* SSID = "MikroTik2G";

void setup() {
  Serial.begin(115200);

  connectToWifi(SSID, WIFI_PASSWORD);

  server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request){
    resetState(&state);

    request->send(200, "text/html", document);
  });

  server.on("/reset", [&](AsyncWebServerRequest *request){
    resetState(&state);

    request->send(200, "text/html", playboxHunk);
  });

  server.on("/mode-two", [&](AsyncWebServerRequest *request){
    state.mode = Two;

    String hunk = String(makeMoveHunk);
    hunk.replace("__PLAYER_NAME__", "Player 1");

    sendHunk(request, &hunk);
  });

  server.on("/mode-one", [&](AsyncWebServerRequest *request){
    state.mode = One;

    String hunk = String(makeMoveHunk);
    hunk.replace("__PLAYER_NAME__", "Player 1");

    sendHunk(request, &hunk);
  });

  server.on("/mode-zero", [&](AsyncWebServerRequest *request){
    state.mode = Zero;
    state.player1Move = makeAIMove(&state);
    state.player2Move = makeAIMove(&state);

    Result result = determineWinner(state.player1Move, state.player2Move);

    String hunk = String(twoAIsHunkResults);
    hunk.replace("__AI_1_MOVE__", getMoveString(state.player1Move));
    hunk.replace("__AI_2_MOVE__", getMoveString(state.player2Move));

    hunk.replace(
      "__RESULT__",
      result == Tie ? "It's a tie" : result == P1 ? "AI 1 wins!" : "AI 2 wins!"
    );

    sendHunk(request, &hunk);
  });

  server.on("/rock", [&](AsyncWebServerRequest *request){
    makeGameMove(request, &state, Rock);
  });

  server.on("/paper", [&](AsyncWebServerRequest *request){
    makeGameMove(request, &state, Paper);
  });

  server.on("/scissors", [&](AsyncWebServerRequest *request){
    makeGameMove(request, &state, Scissors);
  });

  server.on("/htmx.js", HTTP_GET, sendHtmx);
  server.onNotFound(sendNotFound);

  server.begin();

  Serial.println("HTTP server started");
}

void loop() {}
