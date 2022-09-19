#include "./types.h"

int main(void)
{
  srand(time(NULL));

  printf("Truco\n\n");
  fflush(stdout);

  card cards[TOTAL_CARDS_NUMBER];
  set_deck(cards);

  player user, cpu;
  int user_tentos = 0;
  int cpu_tentos = 0;

  user.player_tentos = &user_tentos;
  cpu.player_tentos = &cpu_tentos;
  while (user_tentos < 12 && cpu_tentos < 12)
  {
    play_hand(cards, &user, &cpu);
  }

  printf("%i %i\n", user_tentos, cpu_tentos);

  return 0;
}
