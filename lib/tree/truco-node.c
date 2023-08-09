#include "../deck/cards.h"
#include "../state/truco-state.h"
#include "./truco-node.h"

moves_available get_untried_moves(node *nd, moves_available *legal_moves, moves_available *untried_mvs)
{
  untried_mvs->quantity = 0;

  for (int i = 0; i < legal_moves->quantity; i++)
  {
    bool found = false;
    card move = legal_moves->list[i];
    node *child = nd->child_node;

    while (child != NULL)
    {
      if (is_same_card(child->move, move))
      {
        found = true;
        break;
      }

      child = child->next_simbling;
    }

    if (!found)
    {
      untried_mvs->list[untried_mvs->quantity++] = move;
    }
  }

  return *untried_mvs;
}

float get_UCB1(node *nd, float exploration)
{
  if (nd->visits == 0)
  {
    return INFINITY;
  }

  float exploitation_factor = (nd->wins / (float)nd->visits);
  float visits_proportion = logf((float)nd->parent_node->visits) / (float)nd->visits;
  float exploration_factor = (exploration * sqrtf(visits_proportion));

  return (exploitation_factor + exploration_factor);
}

node *UCB_select_child(node *nd, float exploration)
{
  float greater_UCB = -1;
  node *child = nd->child_node;
  node *best_child = child;

  while (child != NULL)
  {
    float ucb = get_UCB1(child, exploration);
    if (ucb > greater_UCB)
    {
      greater_UCB = ucb;
      best_child = child;
    }

    child->avails += 1;
    child = child->next_simbling;
  }

  return best_child;
}

node *add_child(node *parent, card move, int player)
{
  node *nd = malloc(sizeof(node));
  nd->parent_node = parent;
  nd->child_node = NULL;
  nd->next_simbling = NULL;
  nd->visits = 0;
  nd->wins = 0;
  nd->move = move;
  nd->player_just_moved = player;
  nd->avails = 1;

  if (parent != NULL)
  {
    node *child = parent->child_node;

    if (child == NULL)
    {
      parent->child_node = nd;
      return nd;
    }

    while (child->next_simbling != NULL)
    {
      // taking the simblings of the child to get the children of the parent
      child = child->next_simbling;
    }

    child->next_simbling = nd;
  }

  return nd;
}

void update(node *nd, trucoState *terminal_state)
{
  nd->visits += 1;
  if (nd->player_just_moved != -1)
  {
    int tentos = terminal_state->playerTentos[nd->player_just_moved - 1];

    if (tentos >= 12)
    {
      nd->wins += 1;
    }
    else
    {
      nd->wins -= 1;
    }

    // nd->wins += get_result(terminal_state, nd->player_just_moved - 1);
  }
}
