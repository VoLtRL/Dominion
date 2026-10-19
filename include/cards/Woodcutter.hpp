#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"


class Woodcutter : public Card, public Action {
    public:
        Woodcutter() : Card("Woodcutter", 3," +1 Buy; +2 Coins.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};