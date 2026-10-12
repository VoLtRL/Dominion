#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"

class Copper : public Card {
private:
public:
  Copper() : Card("Copper", 0, "+1 gold", {CardType::TREASURE}) {}
  void onPlay(Game &g, Player &p) const;
};
