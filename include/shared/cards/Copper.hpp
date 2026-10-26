#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Treasure.hpp"

class Copper : public Card, public Treasure {
private:
public:
  Copper() : Card("Copper", 0, "+1 gold", {CardType::TREASURE}, Extension::BASE_CARDS) {}
  int getCoins() const;
};
