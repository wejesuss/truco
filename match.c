#include "./types.h"

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

int show_player_cards(card *player_cards);
void show_instruction(int available);
card ask_cpu_for_card(card *cpu_cards);
card ask_user_for_card(card *user_cards);
void ask_cards_from_players(bool is_user_foot,
                            card *user_cards, card *cpu_cards,
                            card *user_card, card *cpu_card,
                            trick *first_trick);
void set_trick_result(card user_card, card cpu_card,
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
  int *cpu_tentos = (*cpu_ptr).player_tentos;

  shuffle_cards(cards);
  // cards array should be pushed here for each player (randomly for the first card)
  if (is_user_foot) // deciding who "shuffled" the cards (who is gonna get cards first)
  {
    draw_cards(cards, cpu_ptr, user_ptr);
  }
  else
  {
    draw_cards(cards, user_ptr, cpu_ptr);
  }

  trick tricks[3];
  int trick = 0;
  enum round_result current_result = TIE;
  bool is_user_turn = !is_user_foot;
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

  set_trick_result(user_card, cpu_card, &trick);

  char cardname[10];
  printf("%s (%i) vs ", get_card_name(cardname, user_card.suit, user_card.rank), user_card.value);
  printf("%s (%i)\n\n", get_card_name(&cardname[5], cpu_card.suit, cpu_card.rank), cpu_card.value);

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

card ask_cpu_for_card(card *cpu_cards)
{
  printf("Cartas do CPU são: ");
  show_player_cards(cpu_cards);

  int index = rand() % TOTAL_HAND_CARDS_NUMBER;
  card card = cpu_cards[index];

  while (!card.available)
  {
    index = rand() % TOTAL_HAND_CARDS_NUMBER;
    card = cpu_cards[index];
  }

  cpu_cards[index].available = false;

  printf("\n");

  return card;
}

card ask_user_for_card(card *user_cards)
{
  printf("Suas cartas são: ");
  int available = show_player_cards(user_cards);

  show_instruction(available);

  int choose = 0, pos = 0, found = 0;
  card card;

  scanf("%i", &choose);
  while (true)
  {
    if (choose < 1 || choose > available)
    {
      show_instruction(available);

      scanf("%i", &choose);
      continue;
    }

    card = user_cards[pos];
    if (card.available)
    {
      found++;
    }

    if (found == choose)
    {
      user_cards[pos].available = false;
      break;
    }

    pos++;
  }

  printf("\n");

  return card;
}

int show_player_cards(card *player_cards)
{
  char cardname[15];
  int available = 0;
  for (size_t i = 0; i < TOTAL_HAND_CARDS_NUMBER; i++)
  {
    card card = player_cards[i];

    if (card.available)
    {
      available++;
      printf("%s ", get_card_name(&cardname[i * 5], card.suit, card.rank));
    }
  }

  printf("\n");

  return available;
}

void show_instruction(int available)
{
  if (available > 1)
  {
    printf("\nEscolha uma carta (1 a %i): ", available);
  }
  else
  {
    printf("\nEscolha uma carta (1): ");
  }
}

void set_trick_result(card user_card, card cpu_card,
                      trick *trick)
{
  if (cpu_card.value > user_card.value)
  {
    trick->result = LOSE;
    trick->is_tied_by_user = false;
  }
  else if (cpu_card.value < user_card.value)
  {
    trick->result = WIN;
    trick->is_tied_by_user = false;
  }
  else
  {
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
