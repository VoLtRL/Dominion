#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Treasure.hpp"

class Copper : public Card, public Treasure {
private:
public:
  Copper() : Card("Copper", 0, "+1 coin", {CardType::TREASURE}) {}
  int getCoins() const;
};
