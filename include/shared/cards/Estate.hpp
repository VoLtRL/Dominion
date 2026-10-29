#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Victory.hpp"


class Estate : public Card, public Victory  {
    public: 
        Estate() : Card("Estate", 2, "", {CardType::VICTORY}, Extension::BASE_CARDS) {}
        int getVictoryPoints(const Player& p) const;
};