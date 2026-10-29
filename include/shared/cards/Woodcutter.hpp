#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"


class Woodcutter : public Card, public Action {
    public:
        Woodcutter() : Card("Woodcutter", 3," +1 Buy; +2 Coins.", {CardType::ACTION}, Extension::DOMINION_REMOVED_CARDS) {}
        void play(Game& g, Player& p) const;
};