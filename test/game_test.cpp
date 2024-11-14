#include <Arduino.h>
#include <unity.h>

#include "../src/game.h"

void test_clear_state() {
  State state;

  resetState(&state);

  TEST_ASSERT(state.mode == Idle);
  TEST_ASSERT(state.player1Move == None);
  TEST_ASSERT(state.player2Move == None);
}

void test_result() {
  TEST_ASSERT(determineWinner(Paper, Paper) == Tie);
  TEST_ASSERT(determineWinner(Paper, Rock) == P1);
  TEST_ASSERT(determineWinner(Rock, Paper) == P2);
}

void test_move_string() {
  TEST_ASSERT(
    String(getMoveString(Rock)) == "Rock"
  );
  TEST_ASSERT(
    String(getMoveString(Paper)) == "Paper"
  );
  TEST_ASSERT(
    String(getMoveString(Scissors)) == "Scissors"
  );
}

void setup() {
  UNITY_BEGIN();

  RUN_TEST(test_clear_state);
  RUN_TEST(test_result);
  RUN_TEST(test_move_string);

  UNITY_END();
}

void loop() {}
