#include "./players.h"

void show_final_victor(int user_tentos, int cpu_tentos);

int main(void)
{
  srand(time(NULL));

  printf("Truco\n\n");
  fflush(stdout);

  card cards[TOTAL_CARDS_NUMBER];
  set_deck(cards);

  player user = get_user(), cpu = get_cpu();
  reset_players();

  while ((*user.player_tentos) < 12 && (*cpu.player_tentos) < 12)
  {
    play_hand(cards, &user, &cpu);
  }

  show_final_victor((*user.player_tentos), (*cpu.player_tentos));
  fflush(stdout);

  return 0;
}

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
