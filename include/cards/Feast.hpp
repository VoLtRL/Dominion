#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"


class Feast : public Card, public Action {
    public:
        Feast() : Card("Feast", 4," Trash this card. Gain a card costing up to 5.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};