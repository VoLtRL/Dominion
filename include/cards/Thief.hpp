#pragma once 
#include "Card.hpp"

class Thief : public Card{
    public:
        Thief() : Card("Thief", 4," Each other player reveals the top 2 cards of their deck, trashes a revealed Treasure you choose, and discards the rest. You may gain any trashed cards.", {CardType::Action, CardType::Attack}) {}
        void onPlay(Game& g, Player& p) override;
};