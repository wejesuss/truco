enum states
{
  END_OF_MATCH = -1,
  IDLE,
  CHECK_USER_TURN,
  SET_ASK_PLAYER_CARD,
  ASK_USER_CARD,
  ASK_CPU_CARD,
  SET_TRICK_RESULT,
  SHOW_PLAYED_CARDS,
  CHECK_WINNER,
  UPDATE_WINNER_TENTOS
};

enum calltruco
{
  NO_PLAYER_ASKING_TRUCO,
  USER_ASKING_TRUCO,
  CPU_ASKING_TRUCO
};

typedef struct state
{
  enum states current_state;
  enum states previous_state;
  // which player is asking truco
  enum calltruco current_asking_player;
  // which player has asked truco
  enum calltruco previous_asking_player;
  // if the two playes were asked a card
  bool asked_two_players;
} state;

state get_state();
state update_state(enum states new_state);
state reset_state();
void set_asked_two_players(enum states new_state);
bool asked_two_players();
enum calltruco get_current_asking_player();
enum calltruco get_previous_asking_player();
void set_asking_player(enum calltruco new_asking_player);
