#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Victory.hpp"

class Duchy : public Card, public Victory  {
    public: 
        Duchy() : Card("Duchy", 5, "", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const;
};