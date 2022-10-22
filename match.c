#include "./players.h"
#include "./state-manager.h"

bool is_user_foot = true;

enum round_result
{
  TIE,
  WIN,
  LOSE
};

typedef struct trick
{
  enum round_result result;
  bool is_tied_by_user;
} trick;

void ask_cards_from_players(bool is_user_foot,
                            card *user_cards, card *cpu_cards,
                            card *user_card, card *cpu_card,
                            trick *first_trick);
void set_trick_result(bool is_user_turn, card user_card, card cpu_card,
                      trick *first_trick);
trick play_trick(player *user_ptr, player *cpu_ptr, bool is_user_turn);
trick play_first_trick(player *user_ptr, player *cpu_ptr);
bool check_user_turn(trick trick);
trick play_second_trick(player *user_ptr, player *cpu_ptr, bool is_user_turn);
trick play_third_trick(player *user_ptr, player *cpu_ptr, bool is_user_turn);
enum round_result check_winner(trick *tricks, bool check_third_trick);

void play_hand(card *cards, player *user_ptr, player *cpu_ptr)
{
  int *user_tentos = (*user_ptr).player_tentos;
  card *user_cards = (*user_ptr).cards;

  int *cpu_tentos = (*cpu_ptr).player_tentos;
  card *cpu_cards = (*cpu_ptr).cards;

  // cards array should be pushed here for each player (randomly for the first card)
  shuffle_cards(cards);
  if (is_user_foot) // deciding who "shuffled" the cards (who is gonna get cards first)
  {
    draw_cards(cards, cpu_ptr, user_ptr);
  }
  else
  {
    draw_cards(cards, user_ptr, cpu_ptr);
  }

  enum states state = get_state().current_state;
  trick tricks[3];
  int trick = 0;
  enum round_result current_result = TIE;
  bool is_user_turn = !is_user_foot;
  card user_card, cpu_card;

  while (trick < 3)
  {
    state = get_state().current_state;

    switch (state)
    {
    case IDLE:
      printf("IDLE\n");
      if (asked_two_players())
      {
        update_state(SET_TRICK_RESULT);
      }
      else
      {
        state = (trick > 0) ? CHECK_USER_TURN : SET_ASK_PLAYER_CARD;
        update_state(state);
      }

      break;

    case CHECK_USER_TURN:
      printf("CHECK_USER_TURN\n");
      is_user_turn = check_user_turn(tricks[trick - 1]);
      update_state(SET_ASK_PLAYER_CARD);
      break;

    case SET_ASK_PLAYER_CARD:
      printf("SET_ASK_PLAYER_CARD\n");
      state = (is_user_turn) ? ASK_USER_CARD : ASK_CPU_CARD;
      update_state(state);
      break;

    case ASK_USER_CARD:
      printf("ASK_USER_CARD\n");
      user_card = ask_user_for_card(user_cards);
      state = get_state().previous_state == ASK_CPU_CARD ? IDLE : ASK_CPU_CARD;
      update_state(state);
      break;

    case ASK_CPU_CARD:
      printf("ASK_CPU_CARD\n");
      cpu_card = ask_cpu_for_card(cpu_cards);
      state = get_state().previous_state == ASK_USER_CARD ? IDLE : ASK_USER_CARD;
      update_state(state);
      break;

    case SET_TRICK_RESULT:
      printf("SET_TRICK_RESULT\n");
      set_trick_result(is_user_turn, user_card, cpu_card, &tricks[trick]);
      update_state(SHOW_PLAYED_CARDS);
      break;

    case SHOW_PLAYED_CARDS:
      printf("SHOW_PLAYED_CARDS\n");
      show_played_cards(user_card, cpu_card);
      update_state(IDLE);
      trick++;
      break;

    default:
      printf("default\n");
      break;
    }
  }

  tricks[trick] = play_trick(user_ptr, cpu_ptr, is_user_turn);

  while (true)
  {
    is_user_turn = check_user_turn(tricks[trick++]);
    tricks[trick] = play_trick(user_ptr, cpu_ptr, is_user_turn);
    current_result = check_winner(tricks, trick < 2 ? false : true);

    if (current_result == WIN)
    {
      (*user_tentos) += 2;
      break;
    }
    else if (current_result == LOSE)
    {
      (*cpu_tentos) += 2;
      break;
    }
    else if (trick == 2)
    {
      // third trick in case of tie-tie-tie
      break;
    }
  }

  // cards should be made available again here
  reset_deck(cards);
  // toggling who will shuffle the cards next
  is_user_foot = !is_user_foot;
}

trick play_trick(player *user_ptr, player *cpu_ptr, bool is_user_turn)
{
  card *user_cards = (*user_ptr).cards;
  card *cpu_cards = (*cpu_ptr).cards;

  card user_card, cpu_card;
  trick trick;

  ask_cards_from_players(is_user_turn,
                         user_cards, cpu_cards,
                         &user_card, &cpu_card,
                         &trick);

  set_trick_result(is_user_turn, user_card, cpu_card, &trick);
  show_played_cards(user_card, cpu_card);

  return trick;
}

void ask_cards_from_players(bool is_user_turn,
                            card *user_cards, card *cpu_cards,
                            card *user_card, card *cpu_card,
                            trick *trick)
{
  if (is_user_turn)
  {
    *user_card = ask_user_for_card(user_cards);
    *cpu_card = ask_cpu_for_card(cpu_cards);
    trick->is_tied_by_user = false;
  }
  else
  {
    *cpu_card = ask_cpu_for_card(cpu_cards);
    *user_card = ask_user_for_card(user_cards);
    trick->is_tied_by_user = true;
  }
}

void set_trick_result(bool is_user_turn, card user_card, card cpu_card,
                      trick *trick)
{
  trick->is_tied_by_user = false;

  if (cpu_card.value > user_card.value)
  {
    trick->result = LOSE;
  }
  else if (cpu_card.value < user_card.value)
  {
    trick->result = WIN;
  }
  else
  {
    if (!is_user_turn)
    {
      trick->is_tied_by_user = true;
    }

    trick->result = TIE;
  }
}

bool check_user_turn(trick trick)
{
  enum round_result result = trick.result;

  switch (result)
  {
  case WIN:
    return true;

  case LOSE:
    return false;

  case TIE:
    return trick.is_tied_by_user;

  default:
    return false;
  }
}

enum round_result check_winner(trick *tricks, bool check_third_trick)
{
  const int USER_VICTORY = 2;
  const int CPU_VICTORY = 4;

  int match_score = 0;

  if (tricks[0].result == TIE)
  {
    match_score += tricks[1].result;
  }
  else
  {
    match_score += tricks[0].result;
  }

  if (tricks[1].result == TIE)
  {
    match_score += tricks[0].result;
  }
  else
  {
    match_score += tricks[1].result;
  }

  if (match_score == USER_VICTORY)
  {
    printf("Você GANHOU essa mão\n\n");

    return WIN;
  }
  else if (match_score == CPU_VICTORY)
  {
    printf("Você PERDEU essa mão\n\n");

    return LOSE;
  }
  else if (check_third_trick)
  {
    // deciding winner using third trick or first if it's a tie
    match_score = tricks[2].result == TIE ? tricks[0].result : tricks[2].result;
    char *result = (match_score == WIN) ? "Você GANHOU essa mão\n" : (match_score == LOSE) ? "Você PERDEU essa mão\n"
                                                                                           : "EMPATE\n";
    printf("%s\n", result);

    return match_score;
  }
  else
  {
    // in case only two tricks were played yet
    return TIE;
  }
}
