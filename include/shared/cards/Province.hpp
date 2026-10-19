#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Victory.hpp"


class Province : public Card, public Victory  {
    public: 
        Province() : Card("Province", 8, "", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const;
};