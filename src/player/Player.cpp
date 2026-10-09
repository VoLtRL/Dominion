#include "../../include/player/Player.hpp"

void Player::addTurnPlayed() {
  turnsPlayed++;
}

void Player::swapAndShuffleStacks() {
  std::swap(drawStack, discardStack);
  drawStack.shuffle();
}

int Player::getScore() const {
  return score;
}

std::string Player::getString() const {
  return "Player";
}