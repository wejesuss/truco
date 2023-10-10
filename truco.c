#include "./lib/types.h"
#include "./lib/deck/cards.h"
#include "./lib/alloc/malloc-list.h"
#include "./lib/state/truco-state.h"
#include "./lib/tree/truco-node.h"
#include "./players.h"

void show_final_victor(int user_tentos, int cpu_tentos);
void show_state(trucoState *state);
void raise_stake(trucoState *state);
bool cpu_asking_truco(trucoState *state);
bool user_asking_truco(trucoState *state);

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

void raise_stake(trucoState *state)
{
  if (state->stake < 12)
  {
    state->stake += state->stake == 4 ? 4 : 2;
  }
}

/// @brief Ask other player for truco, reasking if needed (from pov of other player)
/// @param state State to be used to sync last asking player and tentos if truco is denied
/// @return True if truco was denied by either player, False otherwise
bool cpu_asking_truco(trucoState *state)
{
  printf("\nAposta: %i\n", state->stake);

  enum truco_options decision = ask_user_truco();
  state->lastAskingPlayer = 2;

  switch (decision)
  {
  case deny:
    state->playerTentos[1] += state->stake;
    deal(state);
    return true;
    break;

  case accept:
    raise_stake(state);
    break;

  case retruco:
    raise_stake(state);
    return user_asking_truco(state);
    break;

  default:
    break;
  }

  return false;
}

/// @brief Ask other player for truco, reasking if needed (from pov of other player)
/// @param state State to be used to sync last asking player and tentos if truco is denied
/// @return True if truco was denied by either player, False otherwise
bool user_asking_truco(trucoState *state)
{
  printf("\nAposta: %i\n", state->stake);

  enum truco_options decision = ask_cpu_truco();
  state->lastAskingPlayer = 1;

  switch (decision)
  {
  case deny:
    state->playerTentos[0] += state->stake;
    deal(state);
    return true;
    break;

  case accept:
    raise_stake(state);
    break;

  case retruco:
    raise_stake(state);
    return cpu_asking_truco(state);
    break;

  default:
    break;
  }

  return false;
}

int main()
{
  printf("Truco\n");
  fflush(stdout);

  srand(time(NULL));
  malloc_list = get_malloc_list(50);
  trucoState rootstate = {
      .stake = 2,
      .currentTrick = 0,
      .playerToMove = 1,
      .lastAskingPlayer = -1,
      .playerTentos = {0, 0}};

  deal(&rootstate);

  moves_available moves = {.quantity = 0, .list = malloc(sizeof(card) * 3)};
  while (get_moves(&rootstate, &moves).quantity != 0)
  {
    show_state(&rootstate);

    card move;
    if (rootstate.playerToMove == 1)
    {
      card *user_cards = rootstate.playerHands[0].cards;
      player_action user_action =
          get_user_action(user_cards,
                          is_hand_of_ten(&rootstate));

      // He can decide to play a card and ask truco/hide card
      // If asked truco, should get an answer from cpu
      if (user_action.asked_truco && rootstate.lastAskingPlayer != 1)
      {
        // TODO: ask cpu truco possibily re-asking in some cases
        // and finishing round if denied
        bool truco_denied = user_asking_truco(&rootstate);
        if (truco_denied)
        {
          continue;
        }
      }

      // if hide card, alter move so that cpu does not see user card
      if (user_action.hid_card)
      {
        hide_card(user_action.choice);
      }

      move = *user_action.choice;
    }
    else
    {
      playerHand *cpu_hand = &rootstate.playerHands[1];
      card *cpu_cards = cpu_hand->cards;
      player_action cpu_action = get_cpu_action(&rootstate, is_hand_of_ten(&rootstate));
      move = MCTS(&rootstate, 1000);

      // printf("truco: %d   hide: %d\n", cpu_action.asked_truco, cpu_action.hid_card);
      // It can decide to play a card and ask truco/hide card
      // If asked truco, should get an answer from user
      if (cpu_action.asked_truco && rootstate.lastAskingPlayer != 2)
      {
        // TODO: ask user truco possibily re-asking in some cases
        // and finishing round if denied
        bool truco_denied = cpu_asking_truco(&rootstate);
        if (truco_denied)
        {
          continue;
        }
      }

      // if hide card, alter move so that user does not see cpu card
      // only hide if cpu will not lose the round
      if (cpu_action.hid_card)
      {
        int pos = findMoveInHand(cpu_hand, move);
        hide_card(&cpu_hand->cards[pos]);
        move = cpu_hand->cards[pos];
      }
    }

    trick last_movement = do_move(&rootstate, move);
    printf("\nLast Movement\nCarta 1: %i | Carta 2: %i\n", last_movement.firstPlay.card.value, last_movement.secondPlay.card.value);
    printf("Resultado %i\n", last_movement.result);
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

void show_state(trucoState *state)
{
  printf("\nVez do jogador: %i", state->playerToMove);
  if (state->playerToMove == 1)
  {
    printf("\nAposta Atual: %i\n", state->stake);
    printf("Mão Atual: %i\n", state->currentTrick + 1);
    printf("Seus Tentos: %i | Tentos do CPU: %i\n", state->playerTentos[0], state->playerTentos[1]);
  }

  show_user_available_cards(state->playerHands[0].cards);

  // show_players_cards(state->playerHands[0].cards, state->playerHands[1].cards);

  for (int trick = 0; trick <= state->currentTrick; trick++)
  {
    card first_card = state->tricks[trick].firstPlay.card;
    card second_card = state->tricks[trick].secondPlay.card;

    // if (first_card.value == -1)
    // {
    //   hide_card(&first_card);
    // }
    // else if (second_card.value == -1)
    // {
    //   hide_card(&second_card);
    // }

    char first_card_name[10];
    char second_card_name[10];

    printf("\nMão %i\nCarta 1: %s | Carta 2: %s\n", trick + 1,
           get_card_name(first_card_name, first_card.suit, first_card.rank),
           get_card_name(second_card_name, second_card.suit, second_card.rank));
    printf("Resultado %i\n", state->tricks[trick].result);
  }
}
