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
player_action get_cpu_action(int choice, card *cpu_cards, bool is_hand_of_ten);
player_action get_user_action(card *user_cards, bool is_hand_of_ten);
void show_instruction(int available, bool is_hand_of_ten);
void show_played_cards(card user_card, card cpu_card);
