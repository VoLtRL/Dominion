#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"


class Feast : public Card, public Action {
    public:
        Feast() : Card("Feast", 4," Trash this card. Gain a card costing up to 5.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};