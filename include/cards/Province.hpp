#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Province : public Card {
    public: 
        Province() : Card("Province", 8, "", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const override;
};