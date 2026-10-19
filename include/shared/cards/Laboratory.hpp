#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"


class Laboratory : public Card, public Action {
    public:
        Laboratory() : Card("Laboratory", 5," +2 Cards; +1 Action.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};