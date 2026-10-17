#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Estate : public Card {
    public: 
        Estate() : Card("Estate", 2, "", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const override;
};