#include "../../include/player/Player.hpp"
#include "../../include/gameplay/Game.hpp"



/**
 * @brief Adds a turn to the player's turn count.
 */
void Player::addTurnPlayed() {
  turnsPlayed++;
}

/**
 * @brief Plays a card from the player's hand.
 * @param card The card to be played.
 * @param g The game instance.
 */
void Player::playCard(const Card* card, Game& g) {
  hand.removeCard(card);
  g.addCardToPlay(card);
  card->onPlay(g, *this);
}

/**
 * @brief Swaps and shuffles the player's draw and discard stacks.
 */
void Player::swapAndShuffleStacks() {
  std::swap(drawStack, discardStack);
  drawStack.shuffle();
}

/**
 * @brief Gets the player's score.
 * @return The player's score.
 */
int Player::getScore() const {
  return score;
}

/**
 * @brief Gets a string representation of the player.
 * @return A string representing the player.
 */
std::string Player::getString() const {
  return name;
}