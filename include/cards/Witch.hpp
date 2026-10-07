#pragma once
#include "Card.hpp"

class Witch : public Card{
    public:
        Witch() : Card("Witch", 5," +2 Cards. Each other player gains a Curse.", {CardType::Action, CardType::Attack}) {}
        void onPlay(Game& g, Player& p) override;
};