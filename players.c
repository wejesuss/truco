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

player_action get_cpu_action(trucoState *state, bool is_hand_of_ten)
{
  card *cpu_cards = state->playerHands[1].cards;

  int available = get_available_cards(cpu_cards);

  player_action cpu_action = {
      .asked_truco = false,
      .hid_card = false};

  if (!is_hand_of_ten)
  {
    // randomize if cpu will ask truco - 40% chance to ask truco
    cpu_action.asked_truco = percentage_random(40);
  }

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
  hand_score /= 3.0;

  // (cpu_tentos - user_tentos) / 12
  float game_score = ((float)state->playerTentos[1] - (float)state->playerTentos[0]) / 12.0;
  float stake_riskiness = (float)state->stake * 0.05; // 0.1/0.2/0.4/0.5
  float trick_score = 0;
  bool will_lose_round = false;

  for (int i = 0; i < state->currentTrick; i++)
  {
    // POV of player 1
    if (state->tricks[i].result == WIN)
    {
      trick_score -= 0.25;
      will_lose_round = true;
    }
    else if (state->tricks[i].result == LOSE)
    {
      trick_score += 0.10;
    }
    else
    {
      trick_score -= 0.10;
    }
  }

  if (will_lose_round)
  {
    cpu_action.hid_card = false;
    return cpu_action;
  }

  stake_riskiness = (game_score > 0) ? 1 - stake_riskiness : 1 + stake_riskiness;
  game_score = game_score * stake_riskiness + trick_score;

  float randomization_chance = ((1 + game_score) * hand_score) / 2;
  int rand_chance = randomization_chance * 100;

  printf("random: %f hand: %f game: %f\n", randomization_chance, hand_score, game_score);

  if (available != 3)
  {
    // randomize if cpu will hide card - rand_chance chance to hide card
    cpu_action.hid_card = percentage_random(rand_chance);
  }

  // TODO: Create heuristic to determinize if cpu is winning or losing this round
  // if winning it CAN ask truco, if cpu has good cards it has 70% chance to ask truco
  // if it does not have good cards but is winning it will only have 40% chance to ask truco
  // if not winning it CAN ask truco if it has good cards with 30% chance to happen
  // otherwise it CAN bluff with 15% chance to happen

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
