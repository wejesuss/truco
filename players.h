#include "types.h"

typedef struct player_action
{
  int choice;
  bool ask_truco;
  bool hide_card;
} player_action;

card ask_cpu_for_card(card *cpu_cards);
card ask_user_for_card(card *user_cards);
int show_player_cards(card *player_cards);
void show_instruction(int available);
void show_played_cards(card user_card, card cpu_card);
