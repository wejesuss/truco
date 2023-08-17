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

// global registry of allocations
alloc_list malloc_list;

card MCTS(trucoState *roostate, int itermax)
{
  moves_available moves = {.quantity = 0, .list = malloc(sizeof(card) * 3)};
  moves_available untried_moves = {.quantity = 0, .list = malloc(sizeof(card) * 3)};
  card rootmove = {.played = true, .value = -1, .rank = -1, .suit = -1};
  node *rootnode = add_child(NULL, rootmove, -1);
  // add allocations to the global registry for organization and simplicity
  add_to_malloc_list(&malloc_list, rootnode);
  add_to_malloc_list(&malloc_list, moves.list);
  add_to_malloc_list(&malloc_list, untried_moves.list);

  for (int i = 0; i < itermax; i++)
  {
    node *node = rootnode;

    // Determinize
    trucoState state = clone_randomizing(roostate, roostate->playerToMove);

    get_moves(&state, &moves);
    get_untried_moves(node, &moves, &untried_moves);

    // Selection
    while (moves.quantity != 0 && untried_moves.quantity == 0) /* node is non-terminal and fully expanded */
    {
      // printf("\n--------------- SELECTION ---------------\n");
      // select ucb child
      node = UCB_select_child(node, 1);
      // do move
      do_move(&state, node->move);

      get_moves(&state, &moves);
      get_untried_moves(node, &moves, &untried_moves);
    }

    // Expansion
    if (untried_moves.quantity != 0) /* if we can expand (state/node is non-terminal) */
    {
      // printf("\n--------------- EXPANSION ---------------\n");
      int pos = rand() % untried_moves.quantity;
      card move = untried_moves.list[pos]; /* select random move */
      int player = state.playerToMove;
      do_move(&state, move);                /* add move to state */
      node = add_child(node, move, player); /* add child to node and descend tree */
      add_to_malloc_list(&malloc_list, node);
    }

    // Simulation
    while (get_moves(&state, &moves).quantity != 0) /* while state is non-terminal */
    {
      // printf("\n--------------- SIMULATION ---------------\n");
      int pos = rand() % moves.quantity;
      card move = moves.list[pos]; /* select random move */
      do_move(&state, move);
    }

    // Backpropagation
    while (node != NULL) /* backpropagate from the expanded node and work back to the root node */
    {
      // printf("\n--------------- BACKPROPAGATE ---------------\n");
      update(node, &state);
      node = node->parent_node;
    }
  }

  // show_node_tree(rootnode);

  int visits = 0;
  card better_move;
  node *child = rootnode->child_node;
  while (child != NULL)
  {
    if (child->visits > visits)
    {
      better_move = child->move;
      visits = child->visits;
    }

    child = child->next_simbling;
  }

  free_malloc_list_members(&malloc_list);

  return better_move;
}

int main()
{
  printf("Truco\n\n");
  fflush(stdout);

  srand(time(NULL));
  malloc_list = get_malloc_list(50);
  trucoState rootstate = {
      .stake = 2,
      .currentTrick = 0,
      .playerToMove = 2,
      .playerTentos = {0, 0}};

  deal(&rootstate);
  show_state(rootstate);

  moves_available moves = {.quantity = 0, .list = malloc(sizeof(card) * 3)};
  while (get_moves(&rootstate, &moves).quantity != 0)
  {
    card move;
    if (rootstate.playerToMove == 1)
    {
      move = MCTS(&rootstate, 500);
    }
    else
    {
      move = MCTS(&rootstate, 100);
    }

    do_move(&rootstate, move);
    show_state(rootstate);
  }

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
