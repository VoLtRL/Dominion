#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"

class Workshop : public Card, public Action {
    public:
        Workshop() : Card("Workshop", 3," Gain a card costing up to 4.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};