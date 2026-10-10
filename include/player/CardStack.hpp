#pragma once

#include "../abstract/Card.hpp"
#include <memory>
#include <stack>

/**
 * @brief Represents a stack of cards.
 * This class provides functionality to manage a stack of cards, including drawing, putting, and shuffling cards.
 */
class CardStack {
private:
  std::stack<const Card*> stack;

public:
  CardStack() = default;

  const std::stack<const Card*> &getStack() const { return stack; }
  int getSize() const { return stack.size(); }

  const Card* drawCard();
  void putCard(const Card* card);

  void shuffle();

};
