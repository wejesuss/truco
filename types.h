typedef struct player
{
  card cards[TOTAL_HAND_CARDS_NUMBER];
  int *player_tentos;
} player;

/*
 prototypes
*/

void play_hand(player *user, player *cpu);
