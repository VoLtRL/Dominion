#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/Action.hpp"

class Chancellor : public Card, public Action  {
    public:
        Chancellor() : Card("Chancellor", 3, "You may immediately put your deck into your discard pile. +2 Coins.", {CardType::ACTION}, Extension::DOMINION_REMOVED_CARDS) {}
        void play(Game& g, Player& p) const;
};