#include "./players.h"

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

void get_choice(int *choice)
{
  scanf("%i", choice);
  char c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

card ask_user_for_card(card *user_cards)
{
  printf("Suas cartas são: ");
  int available = show_player_cards(user_cards);

  show_instruction(available);

  int choice = 0, pos = 0, found = 0;
  card card;

  get_choice(&choice);
  while (true)
  {
    if (choice < 1 || choice > available)
    {
      show_instruction(available);

      get_choice(&choice);
      continue;
    }

    card = user_cards[pos];
    if (card.available)
    {
      found++;
    }

    if (found == choice)
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

void show_played_cards(card user_card, card cpu_card)
{
  char cardname[10];
  printf("%s (%i) vs ", get_card_name(cardname, user_card.suit, user_card.rank), user_card.value);
  printf("%s (%i)\n\n", get_card_name(&cardname[5], cpu_card.suit, cpu_card.rank), cpu_card.value);
}
