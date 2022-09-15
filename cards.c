#include "./types.h"

card create_card(bool available, enum rank rank, enum suits suit, int value)
{
  card new_card = {
      .available = available,
      .rank = rank,
      .suit = suit,
      .value = value};

  return new_card;
}

void set_deck(card *cards)
{
  for (size_t i = four, j = 0; i <= three; i++)
  {
    card card1 = create_card(true, i, spades, i);
    card card2 = create_card(true, i, hearts, i);
    card card3 = create_card(true, i, diamonds, i);
    card card4 = create_card(true, i, clubs, i);

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
    cards[j++] = card1;
    cards[j++] = card2;
    cards[j++] = card3;
    cards[j++] = card4;
  }
}

char *get_card_name(char cardname[5], enum suits suit, enum rank rank)
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
