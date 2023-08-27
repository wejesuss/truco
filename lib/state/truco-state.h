#define TOTAL_CARDS_NUMBER 40
#define TOTAL_HAND_CARDS_NUMBER 3
#define NUMBER_OF_PLAYERS 2
#define MAX_NUMBER_OF_TRICKS 3
#define MAX_PLAYER_TENTOS 12

enum trick_result
{
  TIE,
  WIN,
  LOSE
};

typedef struct playerHand
{
  card cards[TOTAL_HAND_CARDS_NUMBER];
} playerHand;

typedef struct trickplay
{
  card card;
  int player;
} trickplay;

typedef struct trick
{
  trickplay firstPlay;
  trickplay secondPlay;
  enum trick_result result;
} trick;

typedef struct trucoState
{
  int stake;
  int playerToMove;
  int lastAskingPlayer;
  int currentTrick;
  trick tricks[MAX_NUMBER_OF_TRICKS];
  playerHand playerHands[NUMBER_OF_PLAYERS]; // cards in player hands
  int playerTentos[NUMBER_OF_PLAYERS];
} trucoState;

/// @brief struct designed to be a generic return value for a card list followed by a quantity value
typedef struct moves_available
{
  // how many moves we have
  int quantity;
  // the list of moves
  card *list;
} moves_available;

/*
 prototypes
*/

// State
trucoState clone(trucoState *state);
trucoState clone_randomizing(trucoState *state, int player);
void deal(trucoState *state);
moves_available get_moves(trucoState *state, moves_available *moves);
int findMoveInHand(playerHand *hand, card move);
void do_move(trucoState *state, card move);
bool is_hand_of_ten(trucoState *state);
int get_result(int tentos);

// Match
void play_hand(card *cards, trucoState *state);
