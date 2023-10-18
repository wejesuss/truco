typedef struct player_action
{
  card *choice;
  bool asked_truco;
  bool hid_card;
} player_action;

enum truco_options
{
  deny,
  accept,
  retruco // ask truco again
};

enum truco_options ask_cpu_truco();
enum truco_options ask_user_truco();
player_action get_cpu_action(trucoState *state, bool is_hand_of_ten);
player_action get_user_action(card *user_cards, int last_asking_player, bool is_hand_of_ten);
void show_instruction(int available, int last_asking_player, bool is_hand_of_ten);
void show_played_cards(card user_card, card cpu_card);
