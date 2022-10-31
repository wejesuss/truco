#include "./players.h"

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

void get_choice(player_action *action, int available)
{
  show_instruction(available);

  // reseting everything, no need to reset choice
  (*action).ask_truco = false;
  (*action).hide_card = false;

  char c;
  while ((c = getchar()))
  {
    if (c == EOF || c == '\n')
    {
      break;
    }

    if (c == 't')
    {
      (*action).ask_truco = true;
      continue;
    }

    if (c == '?' && available < 3)
    {
      (*action).hide_card = true;
      continue;
    }

    if (c >= '1' && c <= '3')
    {
      // converting character to a number
      (*action).choice = c - '0';
    }
  }
}

card ask_user_for_card(card *user_cards)
{
  printf("Suas cartas são: ");
  int available = show_player_cards(user_cards);
  printf("\n");

  player_action action = {
      .choice = 0,
      .ask_truco = false,
      .hide_card = false};

  int pos = 0, found = 0;
  card card;

  while (action.choice < 1 || action.choice > available)
  {
    get_choice(&action, available);
  }

  // get card from hand
  while (true)
  {
    card = user_cards[pos];
    if (card.available)
    {
      found++;
    }

    if (found == action.choice)
    {
      user_cards[pos].available = false;
      break;
    }

    pos++;
  }

  if (action.hide_card)
  {
    card.value = facedown;
    card.rank = facedown;
    card.suit = facedown;
  }

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
  printf("\nPeça truco com 't'");
  if (available != 3)
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
