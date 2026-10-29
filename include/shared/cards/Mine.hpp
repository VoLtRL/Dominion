#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"

class Mine : public Card, public Action {
    public:
        Mine() : Card("Mine", 5, "Trash a Treasure card from your hand. Gain a Treasure card costing up to 3 more than it.", {CardType::ACTION}, Extension::DOMINION) {}
        void play(Game& g, Player& p) const;
};