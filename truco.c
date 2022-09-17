#include "./types.h"

int main(void)
{
  srand(time(NULL));

  printf("Truco\n\n");
  fflush(stdout);

  card cards[TOTAL_CARDS_NUMBER];
  set_deck(cards);

  int user_tentos = 0;
  int cpu_tentos = 0;
  while (user_tentos < 12 && cpu_tentos < 12)
  {
    play_hand(cards, &user_tentos, &cpu_tentos);
  }

  char cardname[15];
  for (size_t i = 0; i < TOTAL_CARDS_NUMBER; i++)
  {
    printf("%s %s %i\n",
           get_card_name(&cardname[0], cards[i].suit, cards[i].rank),
           get_card_name(&cardname[5], card_down, card_down),
           cards[i].value);
  }

  printf("%i %i\n", user_tentos, cpu_tentos);

  return 0;
}
