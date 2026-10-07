#include "../../include/player/Player.hpp"

void Player::swapAndShuffleStacks() {
  std::swap(drawStack, discardStack);
  drawStack->shuffle();
}
