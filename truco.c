#include "./types.h"

int main(void)
{
  printf("Truco\n\n");
  fflush(stdout);

  card cards[TOTAL_CARDS_NUMBER];
  set_deck(cards);

  for (size_t i = 0; i < TOTAL_CARDS_NUMBER; i++)
  {
    printf("%i %i %i %i\n", cards[i].available, cards[i].rank, cards[i].suit, cards[i].value);
  }

  return 0;
}
