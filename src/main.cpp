#include <Arduino.h>

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <htmx.h>

#include "hunks.h"

enum Result {Tie, P1, P2};
enum Mode {Idle, Zero, One, Two};
enum Move {None, Rock, Paper, Scissors};

typedef struct {
  Mode mode;
  Move player1Move;
  Move player2Move;
} State;

const char* ssid = "ESP32";
const char* password = "12341234";

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);
State state;

void sendHunk(AsyncWebServerRequest *request, String *hunk) {
  request->send(200, "text/html", hunk->begin());
}

char rockMove[] = "Rock";
char paperMove[] = "Paper";
char scissorsMove[] = "Scissors";
char* getMoveString(Move move) {
  if (move == Rock) return rockMove;
  if (move == Paper) return paperMove;
  if (move == Scissors) return scissorsMove;
  return rockMove;
}

Move makeAIMove(State* state) {
  return (Move)random(1, 4);
}

Result determineWinner(Move m1, Move m2) {
  if (m1 == m2) {
    return Tie;
  }

  if (m1 == Rock) {
    if (m2 == Scissors) return P1;
    if (m2 == Paper) return P2;
  } else if (m1 == Paper) {
    if (m2 == Rock) return P1;
    if (m2 == Scissors) return P2;
  } else {
    if (m2 == Paper) return P1;
    if (m2 == Rock) return P2;
  }

  return Tie;
}

void makePlayer1Move(AsyncWebServerRequest *request, State *state, Move move) {
  state->player1Move = move;

  String hunk = String(makeMoveHunk);
  hunk.replace("__PLAYER_NAME__", "Player 2");

  sendHunk(request, &hunk);
}

void makePlayer2Move(AsyncWebServerRequest *request, State *state, Move move) {
  state->player2Move = move;

  const Result result = determineWinner(state->player1Move, state->player2Move);

  String hunk = String(resultsHunk);
  hunk.replace(
    "__RESULT__",
    result == Tie ? "It's a tie" : result == P1 ? "Player 1 wins!" : "Player 2 wins!"
  );

  sendHunk(request, &hunk);
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
        makePlayer1Move(request, state, move);
      } else {
        makePlayer2Move(request, state, move);
      }
      break;
    }
  }
}

void resetState(State *state) {
  state->mode = Idle;
  state->player1Move = None;
  state->player2Move = None;
}

void sendNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void sendHtmx(AsyncWebServerRequest *request) {
  // htmx is a bit too large to send in one chunk, so we need to split it and send in smaller chunks
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
