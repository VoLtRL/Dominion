#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"


class Laboratory : public Card, public Action {
    public:
        Laboratory() : Card("Laboratory", 5," +2 Cards; +1 Action.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};