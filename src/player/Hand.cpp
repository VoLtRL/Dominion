#include "../../include/player/Hand.hpp"

void Hand::addCard(std::shared_ptr<Card> card) {
  hand.push_back(card);
}

void Hand::removeCard(std::shared_ptr<Card> to_rem) {
  for(int i = 0; i < hand.size(); i++) {
    if(hand[i] == to_rem) {
      hand.erase(hand.begin() + i);
      break;
    }
  }
}

const std::vector<std::shared_ptr<Card>> &Hand::getSpecificCards(const CardType &type) const {
  std::vector<std::shared_ptr<Card>> specificCards;
  for (const std::shared_ptr<Card> &card : hand) {
    for(const CardType &cardType : card->getTypes()) {
      if (cardType == type) {
        specificCards.push_back(card);
        break;
      }
    }
  }
  return specificCards;
}

void Hand::displayHand() {}

