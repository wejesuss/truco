#include <stdlib.h>
#include <stdbool.h>

#define TOTAL_CARDS_NUMBER 40
#define TOTAL_HAND_CARDS_NUMBER 3
#define NUMBER_OF_PLAYERS 2

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
  bool played;
  enum rank rank;
  enum suits suit;
  int value;
} card;

/*
 prototypes
*/

// Deck
void set_deck();
card *get_deck();
void draw_cards(card *user_cards, card *cpu_cards);
// void hide_card(card *player_card);
card create_card(bool available, enum rank rank, enum suits suit, int value);
// card get_card_from_hand(card *player_cards, int choice);
bool is_same_card(card reference, card move);
char *get_card_name(char cardname[5], enum suits suit, enum rank rank);
card get_card(int index, int limit);
// Shuffle algorithm by Ben Pfaff's Writings
// see https://benpfaff.org/writings/clc/shuffle.html
void shuffle_cards();
