#include "CardStack.hpp"
#include "Hand.hpp"
#include <iostream>

class Player {
private:
  Hand hand;
  CardStack drawStack;
  CardStack discardStack;

  int turnsPlayed = 0;
  int score = 0;

public:
  Player(Hand a_hand, CardStack a_drawStack,CardStack a_discardStack)
      : hand(std::move(a_hand)), drawStack(std::move(a_drawStack)), discardStack(std::move(a_discardStack)) {

        };
  const Hand &getHand() const { return hand; }
  const CardStack &getDrawStack() const { return drawStack; }
  const CardStack &getDiscardStack() const { return discardStack; }
  void addTurnPlayed();
  int getTurnsPlayed() const { return turnsPlayed; }

  std::string getString() const;

  void discardCard(const Card* card) {
    hand.removeCard(card);
    discardStack.putCard(card);
  }

  void drawCard() {
    if (drawStack.getSize() == 0) {
      swapAndShuffleStacks();
    }
    const Card* drawnCard = drawStack.drawCard();
    hand.addCard(drawnCard);
  }

  void playCard(const Card* card, Game& g) {
    hand.removeCard(card);
    g.addCardToPlay(card);
    card->onPlay(g, *this);
  }

  void addCardToDiscardStack(const Card* card) {
    discardStack.putCard(card);
  }

  void addCardToHand(const Card* card) {
    hand.addCard(card);
  }

  int getScore() const;

  void swapAndShuffleStacks();

  std::string getString() const;
};
