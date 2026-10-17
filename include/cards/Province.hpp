#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Victory.hpp"


class Province : public Card, public Victory  {
    public: 
        Province() : Card("Province", 8, "", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const;
};