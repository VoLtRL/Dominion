#pragma once

#include "../abstract/Card.hpp"
#include "CardStack.hpp"
#include <memory>
#include <vector>

/**
 * @brief Represents a player's hand.
 * This class provides functionality to manage a player's hand of cards.
 */
class Hand {
private:
  std::vector<const Card*> hand;

public:
  Hand() = default;

  Hand(std::vector<const Card*> a_hand) : hand(a_hand) {}

  /**
   * @brief Gets the cards in the hand.
   * @return A constant reference to the vector of cards in the hand.
   */
  const std::vector<const Card*> &getCards() const { return hand; }

  /**
   * @brief Gets the number of cards having a specific type in the hand.
   * @param type The type of card to count.
   * @return The number of cards of the specified type in the hand.
   */
  std::vector<const Card*> getSpecificCards(const CardType &type) const;

  /**
   * @brief Adds a card to the hand.
   * @param card The card to be added.
   */
  void addCard(const Card* card);

  /**
   * @brief Removes a card from the hand.
   * @param card The card to be removed.
   */
  void removeCard(const Card* card);

  /**
   * @brief Adds a card to the player's hand.
   * @param card The card to be added.
   */
  void addCard(const Card* card);

  /**
   * @brief Displays the cards in the player's hand.
   */
  void displayHand() const;

  /**
   * @brief Gets the number of cards in the hand.
   * @return The number of cards in the hand.
   */
  size_t getSize() const { return hand.size(); }

  /**
   * @brief Refills the hand from the draw stack, using the discard stack if necessary.
   * @param drawStack The player's draw stack.
   * @param discardStack The player's discard stack.
   */
  void refillHandFromDrawStack(CardStack& drawStack, CardStack& discardStack);
};
