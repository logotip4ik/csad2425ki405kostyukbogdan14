/**
 * @file game.h
 *
 * @brief Utility file
 */

#include <Arduino.h>

/** @brief enum to represent game result */
enum Result {Tie, P1, P2};
/** @brief enum to represent game mode. Always should start from Idle */
enum Mode {Idle, Zero, One, Two};
/** @brief enum to represent which move player or ai has made */
enum Move {None, Rock, Paper, Scissors};

/**
  * @brief Struct used to represent entire game state.
  */
typedef struct {
  Mode mode;
  Move player1Move;
  Move player2Move;
} State;


char rockMove[] = "Rock";
char paperMove[] = "Paper";
char scissorsMove[] = "Scissors";
/**
  * @param move - move which needs to be translated
  *
  * @brief Returns string representation of move enum
  */
char* getMoveString(Move move) {
  if (move == Rock) return rockMove;
  if (move == Paper) return paperMove;
  if (move == Scissors) return scissorsMove;
  return rockMove;
}

/**
  * @brief makes AI move (random in this case)
  */
Move makeAIMove() {
  return (Move)random(1, 4);
}

/**
  * @param m1 - move of the first player
  * @param m2 - move of the second player
  * @brief Makes decision which player has won
  */
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

/**
  * @param state - current game state pointer
  * @brief Resets the game state struct
  */
void resetState(State *state) {
  state->mode = Idle;
  state->player1Move = None;
  state->player2Move = None;
}
