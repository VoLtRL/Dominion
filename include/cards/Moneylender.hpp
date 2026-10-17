#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"

class Moneylender : public Card, public Action  {
    public:
        Moneylender() : Card("Moneylender", 4, "Trash a Copper card from your hand. +3 Coins.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};