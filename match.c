#include "./types.h"

void play_hand(card *cards, player *user_ptr, player *cpu_ptr)
{
  int *user_tentos = (*user_ptr).player_tentos;
  int *cpu_tentos = (*cpu_ptr).player_tentos;

  shuffle_cards(cards);
  // cards array should be pushed here for each player
  //  (randomly for the first card)
  draw_cards(cards, user_ptr, cpu_ptr);

  // cards should be made available again here
  reset_deck(cards);

  (*user_tentos) += 2;
}
