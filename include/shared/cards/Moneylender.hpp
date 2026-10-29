#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"

class Moneylender : public Card, public Action  {
    public:
        Moneylender() : Card("Moneylender", 4, "Trash a Copper card from your hand. +3 Coins.", {CardType::ACTION}, Extension::DOMINION) {}
        void play(Game& g, Player& p) const;
};