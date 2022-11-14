#include "./players.h"
#include "./state-manager.h"

#define str(x) #x
#define xstr(x) str(x)

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

void raise_stake(int *current_stake);
bool check_user_turn(trick trick);
void set_trick_result(bool is_user_turn, card user_card, card cpu_card,
                      trick *first_trick);
enum round_result check_winner(trick *tricks, bool check_third_trick);

state idle(state match_state, int trick)
{
  if (match_state.asked_two_players)
  {
    match_state.next_state = SET_TRICK_RESULT;
  }
  else
  {
    match_state.next_state = (trick > 0) ? CHECK_USER_TURN : SET_ASK_PLAYER_CARD;
  }

  return match_state;
}

state ask_user_card(state match_state,
                    card *user_cards,
                    card *user_card,
                    enum round_result *current_result,
                    int *stake)
{
  player_action user_action = get_user_action(user_cards);

  if (user_action.asked_truco)
  {
    enum truco_options option;

    match_state = set_asking_player(USER_ASKING_TRUCO);
    enum calltruco current_asking_player = match_state.current_asking_player;
    enum calltruco previous_asking_player = match_state.previous_asking_player;

    while (current_asking_player != previous_asking_player)
    {
      if (current_asking_player == USER_ASKING_TRUCO)
      {
        printf("%s\n\n", xstr(USER_ASKING_TRUCO));
        option = ask_cpu_truco();
      }
      else
      {
        printf("%s\n\n", xstr(CPU_ASKING_TRUCO));
        option = ask_user_truco();
      }

      if (option == deny)
      {
        printf("%s\n\n", xstr(deny));
        if (current_asking_player == USER_ASKING_TRUCO)
        {
          // set user as winner
          printf("%s\n\n", xstr(WIN));
          *current_result = WIN;
        }
        else
        {
          // set cpu as winner
          printf("%s\n\n", xstr(LOSE));
          *current_result = LOSE;
        }

        // will be set when we're out
        match_state.next_state = UPDATE_WINNER_TENTOS;
        break;
      }

      // accept or retruco
      raise_stake(stake);
      printf("aposta vale %i\n", *stake);

      if (option != retruco)
      {
        printf("%s\n\n", xstr(accept));
        break;
      }

      printf("%s\n\n", xstr(retruco));
      enum calltruco new_asking_player = current_asking_player == USER_ASKING_TRUCO
                                             ? CPU_ASKING_TRUCO
                                             : USER_ASKING_TRUCO;
      match_state = set_asking_player(new_asking_player);
      previous_asking_player = match_state.previous_asking_player;
      current_asking_player = match_state.current_asking_player;
    }

    if (option == deny)
    {
      return match_state;
    }
  }

  // choice first
  int pos = 0, found = 0;
  // get card from hand
  while (true)
  {
    *user_card = user_cards[pos];
    if ((*user_card).available)
    {
      found++;
    }

    if (found == user_action.choice)
    {
      user_cards[pos].available = false;
      break;
    }

    pos++;
  }

  if (user_action.hid_card)
  {
    // hide after
    (*user_card).rank = facedown;
    (*user_card).suit = facedown;
    (*user_card).value = facedown;
  }

  // will be set when we're out
  match_state.next_state = match_state.previous_state == ASK_CPU_CARD
                               ? IDLE
                               : ASK_CPU_CARD;

  return match_state;
}

enum states check_match_winner(trick *tricks, int *trick, enum round_result *current_result)
{
  // if it's second or third round
  if ((*trick) >= 1)
  {
    (*current_result) = check_winner(tricks, (*trick) < 2 ? false : true);
    return UPDATE_WINNER_TENTOS;
  }
  // we don't have a winner with just one round
  else
  {
    (*trick)++;
    return IDLE;
  }
}

enum states update_winner_tentos(int *user_tentos,
                                 int *cpu_tentos,
                                 int trick,
                                 enum round_result current_result,
                                 int stake)
{
  // deciding who has won (if so), updating the score
  enum states next_state = IDLE;
  // user wins
  if (current_result == WIN)
  {
    (*user_tentos) += stake;
    next_state = END_OF_MATCH;
  }
  // cpu wins
  else if (current_result == LOSE)
  {
    (*cpu_tentos) += stake;
    next_state = END_OF_MATCH;
  }
  // it's a tie; tie-tie-tie
  else if (trick == 2)
  {
    next_state = END_OF_MATCH;
  }

  // game continues if no one has won
  // on the second trick
  return next_state;
}

void play_hand(card *cards, player *user_ptr, player *cpu_ptr)
{
  int *user_tentos = (*user_ptr).player_tentos;
  card *user_cards = (*user_ptr).cards;

  int *cpu_tentos = (*cpu_ptr).player_tentos;
  card *cpu_cards = (*cpu_ptr).cards;

  int stake = 2;

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

  state match_state = get_state();
  trick tricks[3];
  int trick = 0;
  enum round_result current_result = TIE;
  bool is_user_turn = !is_user_foot;
  card user_card, cpu_card;

  while (match_state.current_state != END_OF_MATCH)
  {
    switch (match_state.current_state)
    {
    case IDLE:
      printf("IDLE\n");
      match_state = idle(match_state, trick);
      update_state(match_state.next_state);
      break;

    case CHECK_USER_TURN:
      printf("CHECK_USER_TURN\n");
      is_user_turn = check_user_turn(tricks[trick - 1]);
      update_state(SET_ASK_PLAYER_CARD);
      break;

    case SET_ASK_PLAYER_CARD:
      printf("SET_ASK_PLAYER_CARD\n");
      update_state((is_user_turn) ? ASK_USER_CARD : ASK_CPU_CARD);
      break;

    case ASK_USER_CARD:
      printf("ASK_USER_CARD\n");
      match_state = ask_user_card(match_state,
                                  user_cards,
                                  &user_card,
                                  &current_result,
                                  &stake);

      update_state(match_state.next_state);
      break;

    case ASK_CPU_CARD:
      printf("ASK_CPU_CARD\n");
      cpu_card = ask_cpu_for_card(cpu_cards);
      update_state(match_state.previous_state == ASK_USER_CARD
                       ? IDLE
                       : ASK_USER_CARD);
      break;

    case SET_TRICK_RESULT:
      printf("SET_TRICK_RESULT\n");
      set_trick_result(is_user_turn,
                       user_card, cpu_card,
                       &tricks[trick]);
      update_state(SHOW_PLAYED_CARDS);
      break;

    case SHOW_PLAYED_CARDS:
      printf("SHOW_PLAYED_CARDS\n");
      show_played_cards(user_card, cpu_card);
      update_state(CHECK_WINNER);
      break;

    case CHECK_WINNER:
      printf("CHECK_WINNER\n");
      match_state.next_state = check_match_winner(tricks, &trick, &current_result);
      update_state(match_state.next_state);
      break;

    case UPDATE_WINNER_TENTOS:
      printf("UPDATE_WINNER_TENTOS\n");
      match_state.next_state = update_winner_tentos(user_tentos, cpu_tentos,
                                                    trick,
                                                    current_result,
                                                    stake);
      update_state(match_state.next_state);
      trick++;
      break;

    default:
      printf("default\n");
      update_state(END_OF_MATCH);
      break;
    }

    match_state = get_state();
  }

  reset_state();

  // cards should be made available again here
  reset_deck(cards);
  // toggling who will shuffle the cards next
  is_user_foot = !is_user_foot;
}

void raise_stake(int *current_stake)
{
  if (*current_stake < 12)
  {
    switch (*current_stake)
    {
    case 4:
      *current_stake += 4;
      break;

    default:
      *current_stake += 2;
      break;
    }
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
