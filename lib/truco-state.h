#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define TOTAL_CARDS_NUMBER 40
#define TOTAL_HAND_CARDS_NUMBER 3
#define NUMBER_OF_PLAYERS 2
#define MAX_NUMBER_OF_TRICKS 3
#define MAX_PLAYER_TENTOS 12

enum trick_result
{
  TIE,
  WIN,
  LOSE
};

enum suits
{
  spades = 1, // (♠)
  hearts,     // (♥)
  diamonds,   // (♦)
  clubs       // (♣)
};

enum rank
{
  four = 1,
  five,
  six,
  seven,
  queen,
  jack,
  king,
  ace,
  two,
  three
};

enum trumpcards // special cards
{
  facedown = 0,
  fourclubs = 14,
  sevenhearts = 13,
  acespades = 12,
  sevendiamonds = 11
};

typedef struct card
{
  enum rank rank;
  enum suits suit;
  int value;
  bool played;
} card;

typedef struct playerHand
{
  card cards[TOTAL_HAND_CARDS_NUMBER];
} playerHand;

typedef struct trickplay
{
  card card;
  int player;
} trickplay;

typedef struct trick
{
  trickplay firstPlay;
  trickplay secondPlay;
  enum trick_result result;
} trick;

typedef struct trucoState
{
  int playerToMove;
  playerHand playerHands[NUMBER_OF_PLAYERS]; // cards in player hands
  int currentTrick;
  trick tricks[MAX_NUMBER_OF_TRICKS];
  int playerTentos[NUMBER_OF_PLAYERS];
} trucoState;

/// @brief struct designed to be a generic return value for a cards list followed by a quantity value
typedef struct moves_available
{
  // how many moves we have
  int quantity;
  // the list of moves
  card *list;
} moves_available;

/*
 prototypes
*/

// State
trucoState clone(trucoState *state);
trucoState clone_randomizing(trucoState *state, int player);
void deal(trucoState *state);
moves_available get_moves(trucoState *state, moves_available *moves);
void do_move(trucoState *state, card move);
int get_result(trucoState *state, int player);

// Deck
void set_deck(card *cards);
void reset_deck(card *cards);
void shuffle_cards(card *cards);
void draw_cards(card *deck, playerHand *user_hand, playerHand *cpu_hand);
void hide_card(card *player_card);
card create_card(bool available, enum rank rank, enum suits suit, int value);
card get_card_from_hand(card *player_cards, int choice);
bool is_same_card(card reference, card move);
char *get_card_name(char cardname[5], enum suits suit, enum rank rank);

// Match
void play_hand(card *cards, trucoState *state);
