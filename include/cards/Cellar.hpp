#pragma once
#include "Card.hpp"

class Cellar : public Card {
    public:
        Cellar() : Card("Cellar", 2, "Discard any number of cards. +1 Card per card discarded.", {CardType::Action}) {}
        void onPlay(Game& g, Player& p) override;
};