#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"

class Workshop : public Card, public Action {
    public:
        Workshop() : Card("Workshop", 3," Gain a card costing up to 4.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};