#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Victory.hpp"


class Estate : public Card, public Victory  {
    public: 
        Estate() : Card("Estate", 2, "", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const;
};