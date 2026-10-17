#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Victory.hpp"

class Duchy : public Card, public Victory  {
    public: 
        Duchy() : Card("Duchy", 5, "", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const;
};