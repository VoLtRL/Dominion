#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"

class Remodel : public Card, public Action  {
    public:
        Remodel() : Card("Remodel", 4, "Trash a card from your hand. Gain a card costing up to 2 more than it.", {CardType::ACTION}, Extension::DOMINION) {}
        void play(Game& g, Player& p) const;
};