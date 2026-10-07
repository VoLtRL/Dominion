#pragma once
#include "Card.hpp"

class Laboratory : public Card{
    public:
        Laboratory() : Card("Laboratory", 5," +2 Cards; +1 Action.", {CardType::Action}) {}
        void onPlay(Game& g, Player& p) override;
};