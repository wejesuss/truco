#define TOTAL_CARDS_NUMBER 40
#define TOTAL_HAND_CARDS_NUMBER 3
#define NUMBER_OF_PLAYERS 2

enum suits
{
  spades = 1, // (♠)
  hearts,     // (♥)
  diamonds,   // (♦)
  clubs       // (♣)
};

enum rank
{
  four = 1,
  five,
  six,
  seven,
  queen,
  jack,
  king,
  ace,
  two,
  three
};

enum trumpcards // special cards
{
  facedown = 0,
  fourclubs = 14,
  sevenhearts = 13,
  acespades = 12,
  sevendiamonds = 11
};

typedef struct card
{
  bool played;
  enum rank rank;
  enum suits suit;
  int value;
} card;

/*
 prototypes
*/

// Deck
void set_deck();
card *get_deck();
void shuffle_cards();
void draw_cards(card *user_cards, card *cpu_cards);
card create_card(bool available, enum rank rank, enum suits suit, int value);
void hide_card(card *player_card);
bool is_same_card(card reference, card move);
card get_card(int index, int limit);
card *get_card_from_hand(card *player_cards, int choice);
void show_user_available_cards(card *user_hand);
void show_players_cards(card *user_hand, card *cpu_hand);
char *get_card_name(char cardname[10], enum suits suit, enum rank rank);
