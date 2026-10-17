#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"

class Curse : public Card {
    public: 
        Curse() : Card("Curse", 0, "", {CardType::CURSE}) {}
        int getVictoryPoints(const Player& p) const override;
};