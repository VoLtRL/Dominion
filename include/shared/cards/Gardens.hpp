#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Victory.hpp"

class Gardens : public Card, public Victory  {
    public:
        Gardens() : Card("Gardens", 4, "Worth 1 Victory Point per 10 cards you have (rounded down).", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const;
};