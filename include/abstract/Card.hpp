#pragma once

#include "CardType.hpp"
#include <set>
#include <string>
#include <vector>

class Player;
class Game;

/**
 * @brief Represents a card in the game.
 * A card has a name, cost, description, and types.
 */
class Card {
private:
  std::string name;
  int cost;
  std::string description;
  std::set<CardType> types;

public:
  Card(std::string cardName, int goldCost, 
       std::string cardDescription, std::set<CardType> cardTypes)
      : name(cardName), cost(goldCost), description(cardDescription), types(cardTypes) {
  }
  /**
   * @brief Gets the name of the card.
   * @return The name of the card.
   */
  const std::string& getName() const { return name; }
  /**
   * @brief Gets the types of the card.
   * @return A set of CardType representing the types of the card.
   */
  const std::set<CardType>& getTypes() const { return types; }
  /**
   * @brief Gets the description of the card.
   * @return The description of the card.
   */
  const std::string& getDescription() const { return description; }
  /**
   * @brief Gets the cost of the card.
   * @return The cost of the card.
   */
  int getCost() const { return cost; }

  /**
   * @brief Called when the card is played.
   * @param g The game instance.
   * @param p The player who played the card.
   */
  virtual void onPlay(Game &g, Player &p) const;
  /**
   * @brief Called when the card is discarded.
   * @param g The game instance.
   * @param p The player who discarded the card.
   */
  virtual void onDiscard(Game &g, Player &p) const;
  /**
   * @brief Called when the card is trashed.
   * @param g The game instance.
   * @param p The player who trashed the card.
   */
  virtual void onTrash(Game &g, Player &p) const;
  /**
   * @brief Called when the card is gained.
   * @param g The game instance.
   * @param p The player who gained the card.
   */
  virtual void onGain(Game &g, Player &p) const;
  /**
   * @brief Checks if the card is of a specific type.
   * @param t The type to check.
   * @return true if the card is of the specified type, false otherwise.
   */
  bool isA(CardType) const;
  /**
   * @brief Gets a string representation of the card.
   * @param maxWidth The maximum width of the string representation.
   * @return A string representing the card.
   */
  const std::string getString(size_t maxWidth = 30) const;
  /**
   * @brief Displays the card's information.
   */
  void display() const;

  /**
   * @brief Checks if the card is empty.
   * @return true if the card is empty, false otherwise.
   */
  bool empty() const { return name.empty(); }
};
