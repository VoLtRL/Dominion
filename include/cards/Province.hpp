#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Victory.hpp"


class Province : public Card, public Victory  {
    public: 
        Province() : Card("Province", 8, "+6 victory points", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const;
};