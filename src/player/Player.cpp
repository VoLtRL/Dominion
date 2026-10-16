#include "../../include/player/Player.hpp"
#include "../../include/gameplay/Game.hpp"

/**
 * @brief Adds a turn to the player's turn count.
 */
void Player::addTurnPlayed() { turnsPlayed++; }

/**
 * @brief Swaps and shuffles the player's draw and discard stacks.
 */
void Player::swapAndShuffleStacks() {
  std::swap(drawStack, discardStack);
  drawStack.shuffle();
}

const Card *Player::drawCard() {
  if (drawStack.getSize() == 0) {
    swapAndShuffleStacks();
  }
  return drawStack.drawCard();
}

/**
 * @brief Gets the player's victory points.
 * @return The player's victory points.
 */
int Player::getVictoryPoints() const {
  int totalVictoryPoints = 0;
  for (const Card *card : hand.getCards()) {
    totalVictoryPoints += card->getVictoryPoints(*this);
  }
  for (std::stack<const Card*> tempStack = drawStack.getStack(); !tempStack.empty(); tempStack.pop()) {
    totalVictoryPoints += tempStack.top()->getVictoryPoints(*this);
  }
  for (std::stack<const Card*> tempStack = discardStack.getStack(); !tempStack.empty(); tempStack.pop()) {
    totalVictoryPoints += tempStack.top()->getVictoryPoints(*this);
  }
  return totalVictoryPoints;
}

/**
 * @brief Discard every cards in the hand of the player
 */
void Player::discardHand() {
  for (auto card : hand.getCards()) {
    discardCard(card);
  }
}
/**
 * @brief Gets a string representation of the player.
 * @return A string representing the player.
 */
std::string Player::getString() const { return name; }

int Player::getTotalCardCount() const {
  return hand.getSize() + drawStack.getSize() + discardStack.getSize();
}

bool Player::hasCardInHand(const Card* card) const{
  for(const Card* c : getHand().getCards()){
    if (card == c){
      return true;
    }
  }
  return false;
}

std::vector<const Reaction*> Player::getAvailableReactions() const{
  std::vector<const Reaction*> reactions;
  for(const Card* card : hand.getCards()){
    if(card->isA(CardType::REACTION)){
      reactions.push_back(dynamic_cast<const Reaction*>(card));
    }
  }
  return reactions;
}
