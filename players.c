#include "./players.h"

player user, cpu;
int user_tentos = 0;
int cpu_tentos = 0;
player_action *user_action = NULL;

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

void reset_user_action()
{
  while (user_action != NULL)
  {
    player_action *current = user_action;
    user_action = user_action->next;
    free(current);
  }
}

player_action get_next_action(player_action *prev_action)
{
  if (prev_action == NULL)
  {
    return *user_action;
  }

  return *prev_action->next;
}

void set_next_user_action(enum action_type type, union action value)
{
  if (user_action == NULL)
  {
    user_action = malloc(sizeof(player_action));
    if (user_action == NULL)
    {
      fprintf(stderr, "Could not create player action\n");
      exit(1);
    }

    user_action->next = NULL;
    user_action->type = type;
    if (type == play_card)
    {
      user_action->value.choice = value.choice;
    }
    else if (type == ask_truco)
    {

      user_action->value.ask_truco = value.ask_truco;
    }
    else
    {
      user_action->value.hide_card = value.hide_card;
    }

    return;
  }

  player_action *current = user_action;
  while (current->next != NULL)
  {
    current = current->next;
  }

  player_action *next_action = malloc(sizeof(player_action));
  if (next_action == NULL)
  {
    fprintf(stderr, "Could not create player action\n");
    exit(1);
  }

  next_action->next = NULL;
  next_action->type = type;
  if (type == play_card)
  {
    next_action->value.choice = value.choice;
  }
  else if (type == ask_truco)
  {

    next_action->value.ask_truco = value.ask_truco;
  }
  else
  {
    next_action->value.hide_card = value.hide_card;
  }

  current->next = next_action;
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

int get_choice(int available)
{
  show_instruction(available);

  reset_user_action();
  int choice = 0;
  bool asked_truco = false;
  bool hid_card = false;

  char c;
  while ((c = getchar()))
  {
    if (c == EOF || c == '\n')
    {
      break;
    }

    if (c == 't' && !is_hand_of_ten() && !asked_truco)
    {
      union action action = {.ask_truco = true};
      set_next_user_action(ask_truco, action);
      asked_truco = true;
      continue;
    }

    if (c == '?' && available != 3 && !is_hand_of_ten() && !hid_card)
    {
      union action action = {.hide_card = true};
      set_next_user_action(hide_card, action);
      hid_card = true;
      continue;
    }

    if (c >= '1' && c <= '3' && choice == 0)
    {
      // converting character to a number
      choice = c - '0';
      union action action = {.choice = choice};
      set_next_user_action(play_card, action);
    }
  }

  return choice;
}

player_action *get_user_action(card *user_cards)
{
  printf("Suas cartas são: ");
  int available = show_player_cards(user_cards);
  printf("\n");

  int choice = 0;
  // int pos = 0, found = 0;
  // card card;

  while (choice < 1 || choice > available)
  {
    choice = get_choice(available);
  }

  // get card from hand
  // while (true)
  // {
  //   card = user_cards[pos];
  //   if (card.available)
  //   {
  //     found++;
  //   }

  //   if (found == choice)
  //   {
  //     user_cards[pos].available = false;
  //     break;
  //   }

  //   pos++;
  // }

  return user_action;
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
