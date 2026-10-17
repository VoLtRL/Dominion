#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"

class Festival : public Card, public Action  {
    public:
        Festival() : Card("Festival", 5, "+2 Actions, +1 Buy, +2 Coins.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};