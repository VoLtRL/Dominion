#include "CardStack.hpp"
#include "Hand.hpp"

class Player {
private:
  Hand hand;
  std::unique_ptr<CardStack> drawStack;
  std::unique_ptr<CardStack> discardStack;

public:
  Player(Hand a_hand, std::unique_ptr<CardStack> a_drawStack, std::unique_ptr<CardStack> a_discardStack)
      : hand(a_hand), drawStack(std::move(a_drawStack)), discardStack(std::move(a_discardStack)) {

        };
  const Hand &getHand() const { return hand; }
  const std::unique_ptr<CardStack> &getDrawStack() const { return drawStack; }
  const std::unique_ptr<CardStack> &getDiscardStack() const { return discardStack; }

  void swapAndShuffleStacks();
};
