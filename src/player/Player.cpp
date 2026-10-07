#include "../../include/player/Player.hpp"

void Player::addTurnPlayed() {
  turnsPlayed++;
}

void Player::swapAndShuffleStacks() {
  std::swap(drawStack, discardStack);
  drawStack.shuffle();
}

int Player::getScore() const {
  int score = 0;
  for (const Card* card : hand.getCards()) {
    if (card->isA(CardType::Victory)) {
      score += card->getScore();
    }
  }
  for (int i = 0; i < drawStack.getSize(); ++i) {
    const Card* card = drawStack.drawCard();
    if (card->isA(CardType::Victory)) {
      score += card->getScore();
    }
  }
  for (int i = 0; i < discardStack.getSize(); ++i) {
    const Card* card = discardStack.drawCard();
    if (card->isA(CardType::Victory)) {
      score += card->getScore();
    }
  }
  return score;
}

std::string Player::getString() const {
  return "Player";
}