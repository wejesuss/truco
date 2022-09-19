#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL_CARDS_NUMBER 40

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

enum trumpcards
{
  facedown = 0,
  fourclubs = 14,
  sevenhearts = 13,
  acespades = 12,
  sevendiamonds = 11
};

typedef struct card
{
  enum suits suit;
  enum rank rank;
  bool available;
  int value;
} card;

typedef struct player
{
  card cards[3];
  int *player_tentos;
} player;

/*
 prototypes
*/

card create_card(bool available, enum rank rank, enum suits suit, int value);
void set_deck(card *cards);
char *get_card_name(char cardname[5], enum suits suit, enum rank rank);
void shuffle_cards(card *cards);
void play_hand(card *cards, player *user, player *cpu);
