#include "./types.h"

typedef struct player_action
{
  int choice;
  bool asked_truco;
  bool hid_card;
} player_action;

enum truco_options
{
  deny,
  accept,
  retruco // ask truco again
};

player get_user();
player get_cpu();
void reset_players();
enum truco_options ask_cpu_truco();
enum truco_options ask_user_truco();
player_action get_cpu_action(card *cpu_cards);
player_action get_user_action(card *user_cards);
int show_player_cards(card *player_cards);
void show_instruction(int available);
void show_played_cards(card user_card, card cpu_card);
