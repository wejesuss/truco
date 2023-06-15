#include "./truco-state.h"

card deck[40];

void show_players_cards(playerHand *user_hand, playerHand *cpu_hand)
{
  printf("\n\nPlayers cards\n");
  for (int i = 0; i < 3; i++)
  {
    printf("%i %i - ", user_hand->cards[i].value, user_hand->cards[i].played);
    printf("%i %i\n", cpu_hand->cards[i].value, cpu_hand->cards[i].played);
  }

  printf("\n");
}

void resetCards(card *cards)
{
  for (int i = 0; i < TOTAL_HAND_CARDS_NUMBER; i++)
  {
    cards[i].played = true;
    cards[i].value = -1;
  }
}

void resetHands(playerHand *playerHands)
{
  resetCards(playerHands[0].cards);
  resetCards(playerHands[1].cards);
}

void resetTricks(trick *tricks)
{
  for (int i = 0; i < MAX_NUMBER_OF_TRICKS; i++)
  {
    tricks[i].result = TIE;

    tricks[i].firstPlay.card.played = false;
    tricks[i].firstPlay.card.value = -1;
    tricks[i].firstPlay.player = -1;

    tricks[i].secondPlay.card.played = false;
    tricks[i].secondPlay.card.value = -1;
    tricks[i].secondPlay.player = -1;
  }
}

/// @brief Check which player won the game
/// @param state Game state to be checked
/// @return Winning (1/2) player, minus one (-1) otherwise
int check_game_winner(trucoState *state)
{
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
  {
    int tentos = state->playerTentos[i];
    if (tentos >= MAX_PLAYER_TENTOS)
    {
      // winning player (non-zero indexed)
      return i + 1;
    }
  }

  return -1;
}

/// @brief Reset state, deck and deal cards to players
/// @param state The state to be resetted
void deal(trucoState *state)
{

  state->currentTrick = 0;
  resetHands(state->playerHands);
  resetTricks(state->tricks);

  bool draw = (check_game_winner(state) == -1) ? true : false;
  if (draw)
  {
    start_deck(deck);
    draw_cards(deck, &state->playerHands[0], &state->playerHands[1]);
  }
}

/// @brief Return the next player
/// @param player The current player, either 1 or 2
/// @return The other player, either 1 or 2
int get_next_player(int player)
{
  return 3 - player;
}

/// @brief Create a deep copy of the given state
/// @param state The state to be copied from
/// @return A new state with same values as state
trucoState clone(trucoState *state)
{
  trucoState copy;
  copy.currentTrick = state->currentTrick;
  copy.playerToMove = state->playerToMove;
  copy.playerTentos[0] = state->playerTentos[0];
  copy.playerTentos[1] = state->playerTentos[1];

  for (int i = 0; i < 3; i++)
  {
    copy.playerHands[0].cards[i] = state->playerHands[0].cards[i];
    copy.playerHands[1].cards[i] = state->playerHands[1].cards[i];

    copy.tricks[i] = state->tricks[i];
  }

  return copy;
}

/// @brief Create a deep copy of the given state
/// randomizing any information not visible to the specified player
/// @param state The state to be copied from
/// @param player The observer player, pov to be used
/// @return A new state with random cards unseen cards
trucoState clone_randomizing(trucoState *state, int player)
{
  trucoState copy = clone(state);

  card *observer_cards = copy.playerHands[(player - 1)].cards;
  card seen_cards[6] = {observer_cards[0], observer_cards[1], observer_cards[2]};
  int seen_card_idx = 3;

  for (int i = 0; i < 3; i++)
  {
    card card = copy.playerHands[get_next_player(player) - 1].cards[i];
    if (card.played)
    {
      seen_cards[seen_card_idx++] = card;
    }
  }

  start_deck(deck);

  for (int i = 0; i < 40; i++)
  {
    for (int j = 0; j < seen_card_idx; j++)
    {
      bool is_same = is_same_card(deck[i], seen_cards[j]);
      if (is_same)
      {
        deck[i].played = true;
        deck[i].value = 0;
      }
    }
  }

  for (int i = 0; i < 3; i++)
  {
    card card = copy.playerHands[get_next_player(player) - 1].cards[i];
    if (card.played)
    {
      continue;
    }

    int pos = 0;
    do
    {
      pos = rand() % 40;
    } while (deck[pos].value == 0);

    copy.playerHands[get_next_player(player) - 1].cards[i] = deck[pos];
    deck[pos].value = 0;
  }

  return copy;
}

/// @brief Get all possible moves from the given state
/// @param state Given state to analyse players hands
/// @param cards Array of cards to be populated
moves_available get_moves(trucoState *state, moves_available *moves)
{
  moves->quantity = 0;
  card *player_cards = state->playerHands[state->playerToMove - 1].cards;

  for (int i = 0; i < 3; i++)
  {
    card card = player_cards[i];
    if (!card.played)
    {
      moves->list[moves->quantity++] = card;
    }
  }

  return *moves;
}

/// @brief Find position in hand.cards that is not used which match move
/// @param hand hand of the player containing an array of cards
/// @param move move to compare with cards in hand
/// @return An int representing the position in cards
int findMoveInHand(playerHand *hand, card move)
{
  for (int i = 0; i < 3; i++)
  {
    card card = hand->cards[i];

    if (card.played == false && is_same_card(card, move))
    {
      return i;
    }
  }
}

/// @brief Return trick result from the pov of player 1
/// @param player The player that did the winning move
/// @return One of trick_result constants
int get_trick_result(int player)
{
  if (player == 1)
  {
    return WIN;
  }
  else if (player == 2)
  {
    return LOSE;
  }
  else
  {
    return TIE;
  }
}

int check_winner(trick *tricks, bool check_third_trick)
{
  const int USER_VICTORY = 2;
  const int CPU_VICTORY = 4;

  int match_score = 0;

  if (tricks[0].result == TIE)
  {
    match_score += tricks[1].result;
  }
  else
  {
    match_score += tricks[0].result;
  }

  if (tricks[1].result == TIE)
  {
    match_score += tricks[0].result;
  }
  else
  {
    match_score += tricks[1].result;
  }

  if (match_score == USER_VICTORY)
  {
    return 0;
  }
  else if (match_score == CPU_VICTORY)
  {
    return 1;
  }
  else if (check_third_trick)
  {
    // deciding winner using third trick or first if it's a tie
    if (tricks[2].result == TIE)
    {
      return tricks[0].result - 1;
    }
    else
    {
      return tricks[2].result - 1;
    }
  }
  else
  {
    // in case only two tricks were played yet
    return -1;
  }
}

/// @brief Update a state by carrying out the given move.
/// Must update playerToMove.
/// @param state The state to be used
/// @param move The move to insert
void do_move(trucoState *state, card move)
{
  trick *tricks = state->tricks;
  int currentTrick = state->currentTrick;
  int playerToMove = state->playerToMove;
  playerHand *playerHand = &state->playerHands[playerToMove - 1];
  bool playFirst = tricks[currentTrick].firstPlay.card.played == false;
  trickplay *play = NULL;
  bool trickIsOver = false;

  if (playFirst)
  {
    play = &tricks[currentTrick].firstPlay;
  }
  else
  {
    play = &tricks[currentTrick].secondPlay;
    trickIsOver = true;
  }

  // play card in current trick
  play->player = playerToMove;
  play->card = move;
  play->card.played = true;

  // removing card from player hand
  int pos = findMoveInHand(playerHand, move);
  playerHand->cards[pos].played = true;

  // get next player
  state->playerToMove = get_next_player(playerToMove);

  // if current trick is secondPlay, then the trick is over
  if (trickIsOver)
  {
    trickplay firstPlay, secondPlay;
    int playsNext;
    firstPlay = tricks[currentTrick].firstPlay;
    secondPlay = tricks[currentTrick].secondPlay;

    if (firstPlay.card.value > secondPlay.card.value)
    {
      playsNext = firstPlay.player;
      tricks[currentTrick].result = get_trick_result(firstPlay.player);
    }
    else if (firstPlay.card.value < secondPlay.card.value)
    {
      playsNext = secondPlay.player;
      tricks[currentTrick].result = get_trick_result(secondPlay.player);
    }
    else
    {
      playsNext = secondPlay.player;
      tricks[currentTrick].result = get_trick_result(0);
    }

    // update game state
    state->currentTrick++;
    state->playerToMove = playsNext;

    // if it's second trick
    if (currentTrick >= 1)
    {
      // check winner in third trick if necessary
      bool third_trick = (currentTrick == 2) ? true : false;
      // check if there is a winner
      int player = check_winner(tricks, third_trick);
      // if so, update tricksTaken
      if (player >= 0)
      {
        state->playerTentos[player] += 2;
        deal(state);
      }
      else if (third_trick)
      {
        // in case three tricks end in a draw (player = -1)
        deal(state);
      }
    }
  }
}

/// @brief Get result of a terminal state from the view point of player
/// @param state Terminal state to analyse
/// @param player player to use as POV
/// @return Number representing how good a state is
int get_result(trucoState *state, int player)
{
  int tentos = state->playerTentos[player];
  if (tentos >= MAX_PLAYER_TENTOS)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}