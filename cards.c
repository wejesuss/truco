#include "./types.h"

card create_card(bool available, enum rank rank, enum suits suit, int value)
{
  card new_card = {
      .available = available,
      .rank = rank,
      .suit = suit,
      .value = value};

  return new_card;
}
