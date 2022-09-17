#include "./types.h"

void play_hand(card *cards, int *user_tentos, int *cpu_tentos)
{
  shuffle_cards(cards);
  (*user_tentos) += 2;
}
