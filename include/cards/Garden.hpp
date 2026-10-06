#pragma once
#include "Cards.hpp"

class Garden : public Card {
    public:
        Garden() : Card("Garden", 4, "Worth 1 Victory Point per 10 cards you have (rounded down).", {CardType::Victory}) {}
        void onPlay(Game& g, Player& p) override;
};