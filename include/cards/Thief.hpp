#pragma once 
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Thief : public Card{
    public:
        Thief() : Card("Thief", 4," Each other player reveals the top 2 cards of their deck, trashes a revealed Treasure you choose, and discards the rest. You may gain any trashed cards.", {CardType::ACTION, CardType::ATTACK}) {}
        void onPlay(Game& g, Player& p) const;
};