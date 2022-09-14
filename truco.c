#include "./types.h"

int main(void)
{
  printf("Truco\n\n");
  fflush(stdout);

  card cards[TOTAL_CARDS_NUMBER];
  for (size_t i = four, j = 0; i <= three; i++)
  {
    card card1;
    card card2;
    card card3;
    card card4;

    card1.available = card2.available = card3.available = card4.available = true;
    card1.rank = card2.rank = card3.rank = card4.rank = i;
    card1.value = card2.value = card3.value = card4.value = i;

    card1.suit = spades;
    card2.suit = hearts;
    card3.suit = diamonds;
    card4.suit = clubs;

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
