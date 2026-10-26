#include "../../include/player/CardStack.hpp"
#include <random>
#include <algorithm>
#include <iostream>
#include <map>
const Card* CardStack::drawCard() {
  if (stack.empty()){return nullptr;}
  auto card = stack.top();
  stack.pop();
  return card;
}

void CardStack::putCard(const Card* card) {
  stack.push(card);
}

void CardStack::shuffle() {
  std::vector<const Card*> cards;
  while (!stack.empty()) {
    cards.push_back(stack.top());
    stack.pop();
  }
  std::shuffle(cards.begin(), cards.end(), std::mt19937{std::random_device{}()});
  for (const Card* card : cards) {
    stack.push(card);
  }
}

std::map<const Card*, int> CardStack::getCardCount() const {
  std::map<const Card*, int> res;
  std::stack<const Card*> clone(stack);
  for(const Card* c = clone.top(); !clone.empty(); clone.pop()){
    res[c]++;
  }
  return res;
}
