#include "./types.h"

void play_hand(card *cards, player *user_ptr, player *cpu_ptr)
{
  int *user_tentos = (*user_ptr).player_tentos;
  int *cpu_tentos = (*cpu_ptr).player_tentos;

  shuffle_cards(cards);
  (*user_tentos) += 2;
}
