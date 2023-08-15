// #include "./types.h"
// #include "./players.h"
// #include "./cards.h"
#include "./lib/types.h"
#include "./lib/deck/cards.h"
#include "./lib/alloc/malloc-list.h"
#include "./lib/state/truco-state.h"
#include "./lib/tree/truco-node.h"

void show_final_victor(int user_tentos, int cpu_tentos);
void show_state(trucoState state);

int main()
{
  printf("Truco\n\n");
  fflush(stdout);

  srand(time(NULL));
  alloc_list malloc_list = get_malloc_list(50);
  printf("%p\n", malloc_list.list);
  trucoState rootstate = {
      .stake = 2,
      .currentTrick = 0,
      .playerToMove = 2,
      .playerTentos = {0, 0}};

  deal(&rootstate);
  show_state(rootstate);

  moves_available moves = {.quantity = 0, .list = malloc(sizeof(card) * 3)};
  // while (get_moves(&rootstate, &moves).quantity != 0)
  // {
  //   card move;
  //   if (rootstate.playerToMove == 1)
  //   {
  //     // move = MCTS(&rootstate, 500);
  //   }
  //   else
  //   {
  //     // move = MCTS(&rootstate, 100);
  //   }

  //   do_move(&rootstate, move);
  // }

  // set_deck();

  // player user = get_user(), cpu = get_cpu();
  // reset_players();

  // while ((*user.player_tentos) < 12 && (*cpu.player_tentos) < 12)
  // {
  //   play_hand(&user, &cpu);
  // }

  // show_final_victor((*user.player_tentos), (*cpu.player_tentos));
  // fflush(stdout);

  free(moves.list);
  unset_malloc_list(&malloc_list);

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

void show_state(trucoState state)
{
  printf("\n -------- STATE --------\n");
  printf("\nStake: %i\n", state.stake);
  printf("\nCurrent Trick: %i\n", state.currentTrick);
  printf("Next Player to Move: %i\n", state.playerToMove);
  printf("User Score: %i | CPU Score: %i\n", state.playerTentos[0], state.playerTentos[1]);
  show_players_cards(state.playerHands[0].cards, state.playerHands[1].cards);

  for (int trick = 0; trick <= state.currentTrick; trick++)
  {
    printf("\nTrick %i\nCard 1: %i | Card 2: %i\n",
           trick,
           state.tricks[trick].firstPlay.card.value,
           state.tricks[trick].secondPlay.card.value);
    printf("Result %i\n", state.tricks[trick].result);
  }
}
