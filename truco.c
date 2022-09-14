#include "./types.h"
#include "./cards.c"

int main(void)
{
  printf("Truco\n\n");
  fflush(stdout);

  card cards[TOTAL_CARDS_NUMBER];
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

  for (size_t i = 0; i < TOTAL_CARDS_NUMBER; i++)
  {
    printf("%i %i %i %i\n", cards[i].available, cards[i].rank, cards[i].suit, cards[i].value);
  }

  return 0;
}
