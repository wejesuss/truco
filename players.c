#include "./lib/types.h"
#include "./lib/deck/cards.h"
#include "./types.h"
#include "./lib/state/truco-state.h"
#include "./players.h"

player user, cpu;

int get_available_cards(card *player_cards)
{
  int available = 0;
  for (int i = 0; i < 3; i++)
  {
    if (!player_cards[i].played)
    {
      available++;
    }
  }

  return available;
}

bool percentage_random(int percentage)
{
  int range = (rand() % 100) + 1;
  printf("range: %i | percentage: %i\n", range, percentage);
  int inclusion_limit = percentage;

  if (range <= inclusion_limit)
  {
    return true;
  }
  else
  {
    return false;
  }
}

float get_hand_score(card *cpu_cards)
{
  // Generate hand score using all cards in hand
  float hand_score = 0;
  for (int i = 0; i < TOTAL_HAND_CARDS_NUMBER; i++)
  {
    if (!cpu_cards[i].played)
    {
      hand_score += (cpu_cards[i].value / 14.0);
    }
  }
  // Calculate average quality of cpu hand
  return (hand_score / 3.0);
}

float get_trick_score(int currentTrick, trick *tricks, bool *will_lose_round)
{
  float trick_score = 0;
  for (int i = 0; i < currentTrick; i++)
  {
    // POV of player 1
    if (tricks[i].result == WIN)
    {
      trick_score -= 0.25;
      *will_lose_round = true;
    }
    else if (tricks[i].result == LOSE)
    {
      trick_score += 0.10;
    }
    else
    {
      trick_score -= 0.10;
    }
  }
}

int get_hid_card_rand_chance(float game_score, float hand_score, float stake_riskiness, float trick_score)
{
  stake_riskiness = (game_score > 0) ? 1 - stake_riskiness : 1 + stake_riskiness;
  game_score = game_score * stake_riskiness + trick_score;

  float randomization_chance = ((1 + game_score) * hand_score) / 2;
  // printf("random: %f hand: %f game: %f\n", randomization_chance, hand_score, game_score);
  return randomization_chance * 100;
}

player_action get_cpu_action(trucoState *state, bool is_hand_of_ten)
{
  card *cpu_cards = state->playerHands[1].cards;

  int available = get_available_cards(cpu_cards);

  player_action cpu_action = {
      .asked_truco = false,
      .hid_card = false};

  bool will_lose_round = false; // if hiding a card will result a defeat
  float hand_score = get_hand_score(cpu_cards);
  // (cpu_tentos - user_tentos) / 12
  float game_score = ((float)state->playerTentos[1] - (float)state->playerTentos[0]) / 12.0;
  float stake_riskiness = (float)state->stake * 0.05; // 0.1/0.2/0.4/0.5
  float trick_score = get_trick_score(state->currentTrick, state->tricks, &will_lose_round);
  int rand_chance = get_hid_card_rand_chance(game_score, hand_score, stake_riskiness, trick_score);

  if (available != 3 && !is_hand_of_ten)
  {
    // randomize if cpu will hide card - {rand_chance} chance to hide card
    cpu_action.hid_card = percentage_random(!will_lose_round ? rand_chance : 0);
  }

  if (!is_hand_of_ten)
  {
    bool user_plays_first = state->tricks[state->currentTrick].firstPlay.player == 1;
    int user_card = state->tricks[state->currentTrick].firstPlay.card.value;
    if (user_plays_first && user_card == 14)
    {
      // prevent cpu from asking truco if user has already played a 4-clubs card
      cpu_action.asked_truco = false;
    }
    else
    {
      // randomize if cpu will ask truco - {rand_chance * 1.4} chance to ask truco
      cpu_action.asked_truco = percentage_random(rand_chance * 1.4);
    }
  }

  return cpu_action;
}

player_action get_choice(card *user_cards, int available, bool is_hand_of_ten)
{
  int choice = 0;
  player_action action = {
      .choice = NULL,
      .asked_truco = false,
      .hid_card = false};

  while (choice < 1 || choice > available)
  {
    show_instruction(available, is_hand_of_ten);
    fflush(stdout);

    choice = 0;
    char c;
    while ((c = getchar()))
    {
      if (c == EOF || c == '\n')
      {
        break;
      }

      if (c == 't' && !is_hand_of_ten)
      {
        action.asked_truco = true;
        continue;
      }

      if (c == '?' && available != 3 && !is_hand_of_ten)
      {
        action.hid_card = true;
        continue;
      }

      if (c >= '1' && c <= '3' && choice == 0)
      {
        // converting character to a number
        choice = c - '0';
      }
    }
  }

  action.choice = get_card_from_hand(user_cards, choice);

  return action;
}

player_action get_user_action(card *user_cards, bool is_hand_of_ten)
{
  int available = get_available_cards(user_cards);

  return get_choice(user_cards, available, is_hand_of_ten);
}

enum truco_options ask_cpu_truco()
{
  // pick a random number - 60% chance to accept truco
  bool accepted = percentage_random(60);

  // use it to decide whether or not truco was accepted
  if (!accepted)
  {
    return deny;
  }

  // if accepted, pick another random number - 30% chance to retruco
  bool ask_truco = percentage_random(30);
  // use it to decide if cpu will retruco
  if (ask_truco)
  {
    return retruco;
  }

  return accept;
}

enum truco_options ask_user_truco()
{
  printf("CPU está chamando truco");
  printf(" deseja correr (n) aceitar (s) ou retrucar (t)?\n");

  enum truco_options option;
  bool valid_answer = false;
  char c;
  while (!valid_answer)
  {
    printf("\nO que quer fazer (n/s/t)? ");
    fflush(stdout);
    c = getchar();

    switch (c)
    {
    case 'n':
      option = deny;
      valid_answer = true;
      break;

    case 's':
      option = accept;
      valid_answer = true;
      break;

    case 't':
      option = retruco;
      valid_answer = true;
      break;

    default:
      break;
    }

    while ((c = getchar()) != EOF && c != '\n')
      ;
  }

  return option;
}

void show_instruction(int available, bool is_hand_of_ten)
{
  if (!is_hand_of_ten)
  {
    printf("\nPeça truco com 't'");

    if (available != 3)
    {
      printf(" esconda uma carta com '?'");
    }
  }

  if (available > 1)
  {
    printf("\nEscolha uma carta (1 a %i): ", available);
  }
  else
  {
    printf("\nEscolha uma carta (1): ");
  }
}

void show_played_cards(card user_card, card cpu_card)
{
  char cardname[10];
  printf("%s (%i) vs ", get_card_name(cardname, user_card.suit, user_card.rank), user_card.value);
  printf("%s (%i)\n\n", get_card_name(&cardname[5], cpu_card.suit, cpu_card.rank), cpu_card.value);
}
