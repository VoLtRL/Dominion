#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/Action.hpp"

class CouncilRoom : public Card, public Action {
    public:
        CouncilRoom() : Card("Council Room", 5, "+4 Cards. +1 Buy. Each other player draws a card.", {CardType::ACTION}, Extension::DOMINION) {}
        void play(Game& g, Player& p) const;
};