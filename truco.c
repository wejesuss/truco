#include "./types.h"

int main(void)
{
  printf("Truco\n\n");
  fflush(stdout);

  card cards[TOTAL_CARDS_NUMBER];
  set_deck(cards);

  char cardname[15];
  for (size_t i = 0; i < TOTAL_CARDS_NUMBER; i++)
  {
    printf("%s %i\n",
           get_card_name(&cardname[5], cards[i].suit, cards[i].value),
           cards[i].value);
  }

  return 0;
}
