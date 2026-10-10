#include "../../include/player/Hand.hpp"
#include <iostream>

void Hand::addCard(const Card* card) {
  hand.push_back(card);
}

void Hand::removeCard(const Card* to_rem) {
  for(size_t i = 0; i < hand.size(); i++) {
    if(hand[i] == to_rem) {
      hand.erase(hand.begin() + i);
      break;
    }
  }
}

std::vector<const Card*> Hand::getSpecificCards(const CardType &type) const {
  std::vector<const Card*> specificCards;
  for (const Card *card : hand) {
    for(const CardType &cardType : card->getTypes()) {
      if (cardType == type) {
        specificCards.push_back(card);
        break;
      }
    }
  }
  return specificCards;
}

void Hand::displayHand() const {
  for (size_t i = 0; i < hand.size(); ++i) {
    std::cout << i + 1 << ". " << hand[i]->getName() << '\n';
  }
}


void Hand::refillHandFromDrawStack(CardStack& drawStack, CardStack& discardStack) {
  while (hand.size() < 5) {
    if (drawStack.getSize() == 0) {
      while (discardStack.getSize() > 0) {
        const Card* card = discardStack.drawCard();
        drawStack.putCard(card);
      }
      drawStack.shuffle();
    }

    if (drawStack.getSize() > 0) {
      const Card* drawnCard = drawStack.drawCard();
      hand.push_back(drawnCard);
    } else {
      break;
    }
  }
}
