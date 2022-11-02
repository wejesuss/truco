#include "./types.h"

union action
{
  int choice;
  bool ask_truco;
  bool hide_card;
};

enum action_type
{
  play_card,
  ask_truco,
  hide_card
};

typedef struct player_action
{
  enum action_type type;
  union action value;
  struct player_action *next;
} player_action;

player get_user();
player get_cpu();
void reset_players();
card ask_cpu_for_card(card *cpu_cards);
player_action *get_user_action(card *user_cards);
void reset_user_action();
card ask_user_for_card(card *user_cards);
int show_player_cards(card *player_cards);
void show_instruction(int available);
void show_played_cards(card user_card, card cpu_card);
