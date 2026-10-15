#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Duchy : public Card {
    public: 
        Duchy() : Card("Duchy", 5, "", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const override;
};