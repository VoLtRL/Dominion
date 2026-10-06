#include "../../include/player/CardStack.hpp"

const std::shared_ptr<Card> CardStack::drawCard() {
  auto card = stack.top();
  stack.pop();
  return card;
}
