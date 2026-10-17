#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/Action.hpp"

class Chancellor : public Card, public Action  {
    public:
        Chancellor() : Card("Chancellor", 3, "You may immediately put your deck into your discard pile. +2 Coins.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};