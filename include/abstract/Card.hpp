#pragma once

#include "../gameplay/Game.hpp"
#include "CardType.hpp"
#include <set>
#include <string>

class Player;

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
  std::string getName() const { return name; }
  std::set<CardType> getTypes() const { return types; }
  std::string getDescription() const { return description; }
  int getCost() const { return cost; }

  virtual void onPlay(Game &g, Player &p) const;
  virtual void onDiscard(Game &g, Player &p) const;
  virtual void onTrash(Game &g, Player &p) const;
  virtual void onGain(Game &g, Player &p) const;
  bool isA(CardType) const;
  std::string getString(size_t maxWidth = 30) const;
  void display() const;

  bool empty() const { return name.empty(); }
};
