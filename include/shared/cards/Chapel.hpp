#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/Action.hpp"

class Chapel : public Card, public Action {
    public:
        Chapel() : Card("Chapel", 2," Trash up to 4 cards from your hand.", {CardType::ACTION}, Extension::DOMINION) {}
        void play(Game& g, Player& p) const;
};