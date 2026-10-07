#include "../abstract/Card.hpp"
#include <memory>
#include <vector>

class Hand {
private:
  std::vector<std::shared_ptr<Card>> hand;

public:
  Hand(std::vector<std::shared_ptr<Card>> a_hand) : hand(a_hand) {}
  const std::vector<std::shared_ptr<Card>> &getCards() const { return hand; }
  const std::vector<std::shared_ptr<Card>> &getSpecificCards(const CardType &type) const;
  void removeCard(std::shared_ptr<Card> card);
  void addCard(std::shared_ptr<Card> card);
  void displayHand();
};
