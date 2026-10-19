#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/Action.hpp"

class Cellar : public Card, public Action {
    public:
        Cellar() : Card("Cellar", 2, "Discard any number of cards. +1 Card per card discarded.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};