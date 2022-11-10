#include "./types.h"
#include "./state-manager.h"

/*
  THIS FILE MANAGES THE STATES FOR THE MATCH
  Its purpose is to control the flow of the program
  when playing three tricks of the game (each match worthing 2-base points)
*/

static state match_state = {
    .current_state = IDLE,
    .previous_state = IDLE,
    .current_asking_player = NO_PLAYER_ASKING_TRUCO,
    .previous_asking_player = NO_PLAYER_ASKING_TRUCO,
    .asked_two_players = false};

state get_state()
{
  return match_state;
}

state update_state(enum states new_state)
{
  set_asked_two_players(new_state);

  match_state.previous_state = match_state.current_state;
  match_state.current_state = new_state;

  return match_state;
}

state reset_state()
{
  match_state.current_state = IDLE;
  match_state.previous_state = IDLE;
  match_state.current_asking_player = NO_PLAYER_ASKING_TRUCO;
  match_state.previous_asking_player = NO_PLAYER_ASKING_TRUCO;
  match_state.asked_two_players = false;

  return match_state;
}

bool asked_two_players()
{
  return match_state.asked_two_players;
}

void set_asked_two_players(enum states new_state)
{
  enum states current_state = match_state.current_state;
  enum states previous_state = match_state.previous_state;

  bool asked_one_player_recently = current_state == ASK_USER_CARD || current_state == ASK_CPU_CARD;
  bool asked_one_player_previously = previous_state == ASK_USER_CARD || previous_state == ASK_CPU_CARD;
  // if two players have already played a card
  match_state.asked_two_players = asked_one_player_recently && asked_one_player_previously;
}

enum calltruco get_current_asking_player()
{
  return match_state.current_asking_player;
}

enum calltruco get_previous_asking_player()
{
  return match_state.previous_asking_player;
}

void set_asking_player(enum calltruco new_asking_player)
{
  match_state.previous_asking_player = match_state.current_asking_player;
  match_state.current_asking_player = new_asking_player;
}
