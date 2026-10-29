#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"

class Market : public Card, public Action  {
    public:
        Market() : Card("Market", 5, "+1 Card, +1 Action, +1 Buy, +1 Coin.", {CardType::ACTION}, Extension::DOMINION){}
        void play(Game& g, Player& p) const;
};