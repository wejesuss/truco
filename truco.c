#include "./types.h"

void show_final_victor(int user_tentos, int cpu_tentos)
{
  if (user_tentos >= 12)
  {
    printf("\nVocê GANHOU o jogo\n");
  }
  else
  {
    printf("\nVocê PERDEU o jogo\n");
  }
}

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

  show_final_victor(user_tentos, cpu_tentos);

  return 0;
}
