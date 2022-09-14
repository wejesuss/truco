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
