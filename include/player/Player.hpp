#pragma once
#include "CardStack.hpp"
#include "Hand.hpp"
#include <iostream>

class Game;

/**
 * @brief Represents a player in the game.
 * This class provides functionality to manage a player's hand, draw stack, and discard stack.
 */
class Player
{
private:
  std::string name;

  Hand hand;
  CardStack drawStack;
  CardStack discardStack;

  int turnsPlayed = 0;
  int score = 0;

public:
  Player(Hand a_hand, CardStack a_drawStack, CardStack a_discardStack)
      : hand(std::move(a_hand)), drawStack(std::move(a_drawStack)), discardStack(std::move(a_discardStack)) {

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
  void discardCard(const Card *card)
  {
    hand.removeCard(card);
    discardStack.putCard(card);
  }

  /**
   * @brief Removes a card from the player's hand.
   * @param card The card to be removed.
   */
  void removeCardFromHand(const Card *card) { hand.removeCard(card); }


  /**
   * @brief Draws a card from the draw stack to the player's hand.
   * If the draw stack is empty, it swaps and shuffles the discard stack into the draw stack.
   */
  void drawCard()
  {
    if (drawStack.getSize() == 0)
    {
      swapAndShuffleStacks();
    }
    const Card *drawnCard = drawStack.drawCard();
    hand.addCard(drawnCard);
  }

  /**
   * @brief Draws a specified number of cards from the draw stack to the player's hand.
   * @param numberOfCards The number of cards to draw.
   */
  void drawCards(int numberOfCards)
  {
    for (int i = 0; i < numberOfCards; ++i)
    {
      drawCard();
    }
  }

  void playCard(const Card *card, Game &g);

  void addCardToDiscardStack(const Card *card)
  {
    discardStack.putCard(card);
  }

  void addCardToHand(const Card *card)
  {
    hand.addCard(card);
  }

  int getScore() const;

  void addVictoryPoints(int points) { score += points; }

  void swapAndShuffleStacks();

  std::string getString() const;
};
