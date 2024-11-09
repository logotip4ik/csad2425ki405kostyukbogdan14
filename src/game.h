#include <Arduino.h>

enum Result {Tie, P1, P2};
enum Mode {Idle, Zero, One, Two};
enum Move {None, Rock, Paper, Scissors};

typedef struct {
  Mode mode;
  Move player1Move;
  Move player2Move;
} State;


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

void resetState(State *state) {
  state->mode = Idle;
  state->player1Move = None;
  state->player2Move = None;
}
