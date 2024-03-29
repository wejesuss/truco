#include "../types.h"
#include "./cards.h"

card deck[40];

card *get_deck()
{
  return deck;
}

card create_card(bool played, enum rank rank, enum suits suit, int value)
{
  card new_card = {
      .played = played,
      .rank = rank,
      .suit = suit,
      .value = value};

  return new_card;
}

void set_deck()
{
  for (size_t i = four, j = 0; i <= three; i++)
  {
    card card1 = create_card(false, i, spades, i);
    card card2 = create_card(false, i, hearts, i);
    card card3 = create_card(false, i, diamonds, i);
    card card4 = create_card(false, i, clubs, i);

    // special cards below
    if (i == four)
    {
      card4.value = fourclubs;
    }

    if (i == seven)
    {
      card2.value = sevenhearts;
      card3.value = sevendiamonds;
    }

    if (i == ace)
    {
      card1.value = acespades;
    }

    // pushing to the deck of cards
    deck[j++] = card1;
    deck[j++] = card2;
    deck[j++] = card3;
    deck[j++] = card4;
  }
}

// Shuffle algorithm by Ben Pfaff's Writings
// see https://benpfaff.org/writings/clc/shuffle.html
void shuffle_cards()
{
  for (size_t i = 0; i < TOTAL_CARDS_NUMBER - 1; i++)
  {
    size_t j = i + rand() / (RAND_MAX / (TOTAL_CARDS_NUMBER - i) + 1);
    if (j != i)
    {
      card t = deck[j];
      deck[j] = deck[i];
      deck[i] = t;
    }
  }
}

card get_card(int index, int limit)
{
  int pos = index >= limit ? (index % limit) : index;

  if (!deck[pos].played)
  {
    card card = deck[pos];
    deck[pos].played = true;

    return card;
  }
  else
  {
    return get_card(index + 1, limit);
  }
}

card *get_card_from_hand(card *player_cards, int choice)
{
  card player_card;
  int pos = 0, found = 0;
  // get found card from choice'th position (choice 1 is first avalable found card)
  while (true)
  {
    player_card = player_cards[pos];
    if (!player_card.played)
    {
      found++;
    }

    if (found == choice)
    {
      break;
    }

    pos++;
  }

  return &player_cards[pos];
}

void hide_card(card *player_card)
{
  (*player_card).rank = facedown;
  (*player_card).suit = facedown;
  (*player_card).value = facedown;
}

void draw_cards(card user_cards[3], card cpu_cards[3])
{
  int start = rand() % TOTAL_CARDS_NUMBER;

  user_cards[0] = get_card(start, TOTAL_CARDS_NUMBER);
  user_cards[1] = get_card(start + 1, TOTAL_CARDS_NUMBER);
  user_cards[2] = get_card(start + 2, TOTAL_CARDS_NUMBER);

  cpu_cards[0] = get_card(start + 3, TOTAL_CARDS_NUMBER);
  cpu_cards[1] = get_card(start + 4, TOTAL_CARDS_NUMBER);
  cpu_cards[2] = get_card(start + 5, TOTAL_CARDS_NUMBER);
}

bool is_same_card(card reference, card move)
{
  bool same_value = reference.value == move.value;
  bool same_look = reference.rank == move.rank && reference.suit == move.suit;

  return (same_value && same_look);
}

char *get_card_name(char cardname[10], enum suits suit, enum rank rank)
{
  switch (rank)
  {
  case three:
    strcpy(cardname, "3");
    break;
  case two:
    strcpy(cardname, "2");
    break;
  case ace:
    strcpy(cardname, "A");
    break;
  case king:
    strcpy(cardname, "K");
    break;
  case jack:
    strcpy(cardname, "J");
    break;
  case queen:
    strcpy(cardname, "Q");
    break;
  case seven:
    strcpy(cardname, "7");
    break;
  case six:
    strcpy(cardname, "6");
    break;
  case five:
    strcpy(cardname, "5");
    break;
  case four:
    strcpy(cardname, "4");
    break;
  default:
    strcpy(cardname, "?");
    break;
  }

  switch (suit)
  {
  case spades:
    strcpy(&cardname[1], "♠");
    break;
  case hearts:
    strcpy(&cardname[1], "♥");
    break;
  case diamonds:
    strcpy(&cardname[1], "♦");
    break;
  case clubs:
    strcpy(&cardname[1], "♣");
    break;
  default:
    strcpy(&cardname[1], "\0");
    break;
  }

  return cardname;
}

void show_user_available_cards(card *user_hand)
{
  bool presented = false;
  char presentation[20] = "\n\nSuas cartas:\n";
  char cardname[10];

  for (int i = 0; i < TOTAL_HAND_CARDS_NUMBER; i++)
  {
    if (!user_hand[i].played)
    {
      if (!presented)
      {
        printf("%s", presentation);
        presented = true;
      }

      printf("%s ", get_card_name(cardname, user_hand[i].suit, user_hand[i].rank));
    }
  }

  printf("\n");
}

void show_players_cards(card *user_hand, card *cpu_hand)
{
  printf("\n\nPlayers cards\n");
  for (int i = 0; i < TOTAL_HAND_CARDS_NUMBER; i++)
  {
    printf("%i %i - ", user_hand[i].value, user_hand[i].played);
    printf("%i %i\n", cpu_hand[i].value, cpu_hand[i].played);
  }

  printf("\n");
}
