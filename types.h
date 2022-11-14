#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TOTAL_CARDS_NUMBER 40
#define TOTAL_HAND_CARDS_NUMBER 3

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
  card cards[TOTAL_HAND_CARDS_NUMBER];
  int *player_tentos;
} player;

/*
 prototypes
*/

card create_card(bool available, enum rank rank, enum suits suit, int value);
card get_card_from_hand(card *user_cards, int choice);
void set_deck(card *cards);
char *get_card_name(char cardname[5], enum suits suit, enum rank rank);
void shuffle_cards(card *cards);
void hide_card(card *user_card);
void play_hand(card *cards, player *user, player *cpu);
void draw_cards(card *cards, player *player1_ptr, player *player2_ptr);
void reset_deck(card *cards);
