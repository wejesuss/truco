#include "./types.h"

typedef struct player_action
{
  int choice;
  bool asked_truco;
  bool asked_truco_first;
  bool hid_card;
} player_action;

player get_user();
player get_cpu();
void reset_players();
card ask_cpu_for_card(card *cpu_cards);
player_action get_user_action(card *user_cards);
int show_player_cards(card *player_cards);
void show_instruction(int available);
void show_played_cards(card user_card, card cpu_card);
