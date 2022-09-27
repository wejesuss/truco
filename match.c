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

trick play_first_trick(player *user_ptr, player *cpu_ptr);

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
  tricks[0] = play_first_trick(user_ptr, cpu_ptr);
  // show player cards
  // ask for a card
  // verify results
  // store on tricks

  // show player cards
  // ask for a card to previous winner (or "cangador")
  // verify results
  // store on tricks

  // cards should be made available again here
  reset_deck(cards);
  // toggling who will shuffle the cards next
  is_user_foot = !is_user_foot;

  (*user_tentos) += 2;
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

card ask_cpu_for_card(card *cpu_cards)
{
  show_player_cards(cpu_cards);

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

card ask_user_for_card(card *user_cards)
{
  printf("\nSuas cartas são: ");
  int available = show_player_cards(user_cards);

  if (available > 1)
  {
    printf("\nEscolha uma carta (1 a %i): ", available);
  }
  else
  {
    printf("\nEscolha uma carta (1): ");
  }

  int choose = 0, pos = 0, found = 0;
  card card;

  scanf("%i", &choose);
  while (true)
  {
    if (choose < 1 || choose > available)
    {
      if (available > 1)
      {
        printf("\nEscolha uma carta (1 a %i): ", available);
      }
      else
      {
        printf("\nEscolha uma carta (1): ");
      }

      scanf("%i", &choose);
      continue;
    }

    card = user_cards[pos];
    if (card.available)
    {
      user_cards[pos].available = false;
      found++;
    }

    if (found == choose)
    {
      break;
    }

    pos++;
  }

  printf("\n");

  return card;
}

trick play_first_trick(player *user_ptr, player *cpu_ptr)
{
  card *user_cards = (*user_ptr).cards;
  card *cpu_cards = (*cpu_ptr).cards;

  card user_card, cpu_card;
  trick first_trick;

  if (is_user_foot)
  {
    cpu_card = ask_cpu_for_card(cpu_cards);
    user_card = ask_user_for_card(user_cards);
    first_trick.is_tied_by_user = true;
  }
  else
  {
    user_card = ask_user_for_card(user_cards);
    cpu_card = ask_cpu_for_card(cpu_cards);
    first_trick.is_tied_by_user = false;
  }

  if (cpu_card.value > user_card.value)
  {
    first_trick.result = LOSE;
    first_trick.is_tied_by_user = false;
  }
  else if (cpu_card.value < user_card.value)
  {
    first_trick.result = WIN;
    first_trick.is_tied_by_user = false;
  }
  else
  {
    first_trick.result = TIE;
  }

  char cardname[10];
  printf("%i %s\n", user_card.value, get_card_name(cardname, user_card.suit, user_card.rank));
  printf("%i %s\n\n", cpu_card.value, get_card_name(&cardname[5], cpu_card.suit, cpu_card.rank));

  return first_trick;
}
