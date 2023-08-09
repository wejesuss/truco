#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

typedef struct node
{
  struct node *parent_node;
  struct node *child_node;
  struct node *next_simbling;
  card move;
  int wins;
  int visits;
  int avails;
  int player_just_moved;
} node;

/// @brief A list of Memory allocations
typedef struct mem_alloc_list
{
  unsigned int quantity;
  unsigned int position;
  void **list;
} alloc_list;

node *add_child(node *parent, card move, int player);
moves_available get_untried_moves(node *nd, moves_available *legal_moves, moves_available *untried_mvs);
node *UCB_select_child(node *nd, float exploration);
void update(node *nd, trucoState *terminal_state);
