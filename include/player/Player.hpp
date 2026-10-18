#pragma once
#include "CardStack.hpp"
#include "Hand.hpp"
#include <iostream>

#include "../abstract/Reaction.hpp"
#include "../events/AttackEvent.hpp"

class Reaction;

class GameEvent;

class Game;

/**
 * @brief Represents a player in the game.
 * This class provides functionality to manage a player's hand, draw stack, and
 * discard stack.
 */
class Player {
private:
  std::string name;

  Hand hand;
  CardStack drawStack;
  CardStack discardStack;

  int turnsPlayed = 0;

public:
  Player(Hand a_hand, CardStack a_drawStack, CardStack a_discardStack,
         std::string playerName)
      : name(playerName), hand(std::move(a_hand)),
        drawStack(std::move(a_drawStack)),
        discardStack(std::move(a_discardStack)) {

        };

  /**
   * @brief Gets the player's hand.
   * @return A reference to the player's hand.
   */
  const Hand &getHand() const { return hand; }

  /**
   * @brief Gets the player's draw stack.
   * @return A reference to the player's draw stack.
   */
  const CardStack &getDrawStack() const { return drawStack; }

  /**
   * @brief Gets the player's discard stack.
   * @return A reference to the player's discard stack.
   */
  const CardStack &getDiscardStack() const { return discardStack; }

  /**
   * @brief Increments the number of turns played by the player.
   */
  void addTurnPlayed();

  /**
   * @brief Gets the number of turns played by the player.
   * @return The number of turns played.
   */
  int getTurnsPlayed() const { return turnsPlayed; }

  /**
   * @brief Discards a card from the player's hand to the discard stack.
   * @param card The card to be discarded.
   */
  void discardCard(const Card *card) {
    hand.removeCard(card);
    discardStack.putCard(card);
  }

  void discardHand();

  /**
   * @brief Removes a card from the player's hand.
   * @param card The card to be removed.
   */
  void removeCardFromHand(const Card *card) { hand.removeCard(card); }

  /**
   * @brief Draws a card from the draw stack.
   * If the draw stack is empty, it swaps and shuffles the discard stack into
   * the draw stack.
   * @return A pointer to the drawn card.
   */
  const Card *drawCard();
  /**
   * @brief Draws a card from the draw stack to the player's hand.
   * If the draw stack is empty, it swaps and shuffles the discard stack into
   * the draw stack.
   */
  void drawCardToHand() {
    if (drawStack.getSize() == 0) {
      swapAndShuffleStacks();
    }
    const Card *drawnCard = drawStack.drawCard();
    if (drawnCard != nullptr) {
      hand.addCard(drawnCard);
    }
  }

  /**
   * @brief Draws a specified number of cards from the draw stack to the
   * player's hand.
   * @param numberOfCards The number of cards to draw.
   */
  void drawCardsToHand(int numberOfCards) {
    for (int i = 0; i < numberOfCards; ++i) {
      drawCardToHand();
    }
  }

  void addCardToDiscardStack(const Card *card) { discardStack.putCard(card); }

  void addCardToHand(const Card *card) { hand.addCard(card); }

  void addCardToDrawStack(const Card *card) { drawStack.putCard(card); }

  int getVictoryPoints() const;

  void swapAndShuffleStacks();

  std::string getString() const;

  bool operator==(const Player &other) const { return &*this == &other; }

  bool operator!=(const Player &other) const { return !(*this == other); }

  const std::string getName() const { return name; }

  void discardDrawStack() {
    while (!drawStack.getStack().empty()) {
      discardStack.putCard(drawStack.drawCard());
    }
  }

  int getTotalCardCount() const;

  bool hasCardInHand(const Card *) const;

  std::vector<const Reaction *> getAvailableReactions() const;
};
