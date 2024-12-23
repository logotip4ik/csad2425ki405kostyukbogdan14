/**
 * @file main.cpp
 *
 * @brief Main hardware file which handles game state, wifi connections and responses.
 */

#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <htmx.h>

#include "hunks.h"
#include "game.h"
#include "env.h"

/** @brief Global web server variable */
AsyncWebServer server(80);
/** @brief Global game state */
State state;

/**
  * @param request - web server request
  * @param hunk - string pointer to send
  * @brief Shortcut for sending string as web server response
  */
void sendHunk(AsyncWebServerRequest *request, String *hunk) {
  request->send(200, "text/html", hunk->begin());
}

/**
  * @param request - web server request
  * @param state - game state pointer
  * @param move - user move
  *
  * @brief Makes desicion on what to send for user in different game modes
  */
void makeGameMove(AsyncWebServerRequest *request, State *state, Move move) {
  switch (state->mode) {
    case One: {
      state->player1Move = move;
      state->player2Move = makeAIMove();

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

/**
  * @param request - web server request
  * @brief Shortcut function to handle 404 requests
  */
void sendNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

/**
  * @param request - web server request
  *
  * Used to send client side js library. Because the response won't fit in single chunk we use `beginChunkedResponse` to send HTMX
  */
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

/**
  * @param ssid - name of the wifi to connect to
  * @param password - password of the wifi
  *
  * Connects to wifi via provided ssid and password. Pauses execution till successful connection.
  */
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

/**
  * Setup which runs when esp32 boots up. This function handles connection to wifi and sending responses to user based on current game state.
  */
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
    state.player1Move = makeAIMove();
    state.player2Move = makeAIMove();

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

/**
* @internal
*/
void loop() {}
