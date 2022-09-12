#include <stdio.h>
#include <stdbool.h>

#define TOTAL_CARDS_NUMBER 40

enum suits
{
  spades,   // (♠)
  hearts,   // (♥)
  diamonds, // (♦)
  clubs     // (♣)
};

enum rank
{
  downcard,
  four,
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

typedef struct card
{
  enum suits suit;
  enum rank rank;
  bool available;
  int value;
} card;
