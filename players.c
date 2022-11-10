#include "./players.h"

player user, cpu;
int user_tentos = 0;
int cpu_tentos = 0;

void reset_players()
{
  user_tentos = cpu_tentos = 0;
}

player get_user()
{
  user.player_tentos = &user_tentos;
  return user;
}

player get_cpu()
{
  cpu.player_tentos = &cpu_tentos;
  return cpu;
}

bool is_hand_of_ten()
{
  if (user_tentos == 10 || cpu_tentos == 10)
  {
    return true;
  }

  return false;
}

card ask_cpu_for_card(card *cpu_cards)
{
  printf("Cartas do CPU são: ");
  show_player_cards(cpu_cards);
  printf("\n");

  int index = rand() % TOTAL_HAND_CARDS_NUMBER;
  card card = cpu_cards[index];

  while (!card.available)
  {
    index = rand() % TOTAL_HAND_CARDS_NUMBER;
    card = cpu_cards[index];
  }

  cpu_cards[index].available = false;

  return card;
}

player_action get_choice(int available)
{
  show_instruction(available);

  player_action action = {
      .choice = 0,
      .asked_truco = false,
      .asked_truco_first = false,
      .hid_card = false};

  char c;
  while ((c = getchar()))
  {
    if (c == EOF || c == '\n')
    {
      break;
    }

    if (c == 't' && !is_hand_of_ten() && !action.asked_truco)
    {
      action.asked_truco = true;

      if (!action.hid_card && action.choice == 0)
      {
        action.asked_truco_first = true;
      }

      continue;
    }

    if (c == '?' && available != 3 && !is_hand_of_ten() && !action.hid_card)
    {
      action.hid_card = true;
      continue;
    }

    if (c >= '1' && c <= '3' && action.choice == 0)
    {
      // converting character to a number
      action.choice = c - '0';
    }
  }

  return action;
}

player_action get_user_action(card *user_cards)
{
  printf("Suas cartas são: ");
  int available = show_player_cards(user_cards);
  printf("\n");

  player_action action = {
      .choice = 0,
      .asked_truco = false,
      .asked_truco_first = false,
      .hid_card = false};

  while (action.choice < 1 || action.choice > available)
  {
    action = get_choice(available);
  }

  return action;
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

enum truco_options ask_cpu_truco()
{
  // pick a random number
  bool accepted = rand() % 2;
  // use it to decide wheter or not truco was accepted
  if (!accepted)
  {
    return deny;
  }

  // if accepted, pick another random number
  bool ask_truco = rand() % 2;
  // use it to decide if cpu will retruco
  if (ask_truco)
  {
    return retruco;
  }

  return accept;
}

enum truco_options ask_user_truco()
{
  printf("CPU está chamando truco");
  printf(" deseja correr (n) aceitar (s) ou retrucar (t)?\n");

  enum truco_options option;
  bool can_stop_getting_char = false;
  char c;
  while (!can_stop_getting_char)
  {
    printf("O que quer fazer (n/s/t)? ");
    c = getchar();

    if (c == 'n')
    {
      option = deny;
      can_stop_getting_char = true;
    }

    if (c == 's')
    {
      option = accept;
      can_stop_getting_char = true;
    }

    if (c == 't')
    {
      option = retruco;
      can_stop_getting_char = true;
    }
  }

  while ((c = getchar()) != EOF && c != '\n')
    ;

  return option;
}

void show_instruction(int available)
{
  printf("\nPeça truco com 't'");
  if (available != 3 && !is_hand_of_ten())
  {
    printf(" esconda uma carta com '?'");
  }

  if (available > 1)
  {
    printf("\nEscolha uma carta (1 a %i): ", available);
  }
  else
  {
    printf("\nEscolha uma carta (1): ");
  }
}

void show_played_cards(card user_card, card cpu_card)
{
  char cardname[10];
  printf("%s (%i) vs ", get_card_name(cardname, user_card.suit, user_card.rank), user_card.value);
  printf("%s (%i)\n\n", get_card_name(&cardname[5], cpu_card.suit, cpu_card.rank), cpu_card.value);
}
