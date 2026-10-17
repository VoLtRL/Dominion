#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"

class Mine : public Card, public Action {
    public:
        Mine() : Card("Mine", 5, "Trash a Treasure card from your hand. Gain a Treasure card costing up to 3 more than it.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};