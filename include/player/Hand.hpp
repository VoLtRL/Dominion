#include "../abstract/Card.hpp"
#include <memory>
#include <vector>

class Hand {
private:
  std::vector<const Card*> hand;

public:
  Hand() = default;
  Hand(std::vector<const Card*> a_hand) : hand(a_hand) {}
  const std::vector<const Card*> &getCards() const { return hand; }
  const std::vector<const Card*> &getSpecificCards(const CardType &type) const;
  void removeCard(const Card* card);
  void addCard(const Card* card);
  void displayHand() const;
  void refillHandFromDrawStack(CardStack& drawStack, CardStack& discardStack);
};
