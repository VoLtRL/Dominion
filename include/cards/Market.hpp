#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"

class Market : public Card, public Action  {
    public:
        Market() : Card("Market", 5, "+1 Card, +1 Action, +1 Buy, +1 Coin.", {CardType::ACTION}){}
        void play(Game& g, Player& p) const;
};