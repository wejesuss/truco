#include "../types.h"
#include "../deck/cards.h"
#include "./truco-state.h"

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
    tricks[i].firstPlay.card.rank = facedown;
    tricks[i].firstPlay.card.suit = facedown;
    tricks[i].firstPlay.player = -1;

    tricks[i].secondPlay.card.played = false;
    tricks[i].secondPlay.card.value = -1;
    tricks[i].secondPlay.card.rank = facedown;
    tricks[i].secondPlay.card.suit = facedown;
    tricks[i].secondPlay.player = -1;
  }
}

/// @brief Get result comparing with maximum amount of tentos
/// @param tentos Amount of tentos player has obtained
/// @return Number representing how good a state is
int get_result(int tentos)
{
  if (tentos >= MAX_PLAYER_TENTOS)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/// @brief Check which player won the game
/// @param state Game state to be checked
/// @return Winning (1/2) player, minus one (-1) otherwise
int check_game_winner(trucoState *state)
{
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++)
  {
    if (get_result(state->playerTentos[i]) == 1)
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
  state->stake = 2;
  state->currentTrick = 0;
  state->lastAskingPlayer = -1;
  resetHands(state->playerHands);
  resetTricks(state->tricks);

  bool draw = (check_game_winner(state) == -1) ? true : false;
  if (draw)
  {
    set_deck();
    shuffle_cards();
    draw_cards(state->playerHands[0].cards, state->playerHands[1].cards);
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
  copy.stake = state->stake;
  copy.currentTrick = state->currentTrick;
  copy.playerToMove = state->playerToMove;
  copy.lastAskingPlayer = state->lastAskingPlayer;
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
/// @return A new state with random unseen cards
trucoState clone_randomizing(trucoState *state, int player)
{
  trucoState copy = clone(state);

  int observer_player = player - 1;
  int adversary_player = get_next_player(player) - 1;

  card *observer_cards = copy.playerHands[observer_player].cards;
  /// Forming what should be called an array of seen card from pov of observer player
  card seen_cards[6] = {observer_cards[0], observer_cards[1], observer_cards[2]};
  int seen_card_idx = 3;

  for (int i = 0; i < TOTAL_HAND_CARDS_NUMBER; i++)
  {
    card card = copy.playerHands[adversary_player].cards[i];
    if (card.played)
    {
      seen_cards[seen_card_idx++] = card;
    }
  }

  // Forming a new deck to redistribute cards to adversary player
  set_deck();
  shuffle_cards();
  card *deck = get_deck();

  for (int i = 0; i < TOTAL_CARDS_NUMBER; i++)
  {
    for (int j = 0; j < seen_card_idx; j++)
    {
      bool is_same = is_same_card(deck[i], seen_cards[j]);
      if (is_same)
      {
        deck[i].played = true;
      }
    }
  }

  int pos = rand() % TOTAL_CARDS_NUMBER;
  for (int i = 0; i < TOTAL_HAND_CARDS_NUMBER; i++)
  {
    card adversary_card = copy.playerHands[adversary_player].cards[i];
    if (adversary_card.played)
    {
      continue;
    }

    card random_card = get_card(pos + i, TOTAL_CARDS_NUMBER);
    copy.playerHands[adversary_player].cards[i] = random_card;
  }

  return copy;
}

/// @brief Get all possible moves from the given state
/// @param state Given state to pull player hand
/// @param moves Array of moves to be populated
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
/// @param trick_is_over Boolean pointer to sinalize if trick has been played by all players
/// @returns a trick struct representing last movements
trick do_move(trucoState *state, card move, bool *trick_is_over)
{
  trick *tricks = state->tricks;
  int currentTrick = state->currentTrick;
  int playerToMove = state->playerToMove;
  playerHand *playerHand = &state->playerHands[playerToMove - 1];
  bool playFirst = tricks[currentTrick].firstPlay.card.played == false;
  trickplay *play = NULL;
  *trick_is_over = false;

  if (playFirst)
  {
    play = &tricks[currentTrick].firstPlay;
  }
  else
  {
    play = &tricks[currentTrick].secondPlay;
    *trick_is_over = true;
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
  if (*trick_is_over)
  {
    trick last_movement;
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

    last_movement = tricks[currentTrick];
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
        state->playerTentos[player] += state->stake;
        deal(state);
      }
      else if (third_trick)
      {
        // in case three tricks end in a draw (player = -1)
        deal(state);
      }
    }

    return last_movement;
  }
  else
  {
    return tricks[currentTrick];
  }
}

/// @brief Check if current round is hand of ten
/// @param state The state to analyse
/// @return True if either player has ten tentos, False otherwise
bool is_hand_of_ten(trucoState *state)
{
  return (state->playerTentos[0] == 10 || state->playerTentos[1] == 10);
}
