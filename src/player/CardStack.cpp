#include "../../include/player/CardStack.hpp"

const Card* CardStack::drawCard() {
  auto card = stack.top();
  stack.pop();
  return card;
}
