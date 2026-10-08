#pragma once
#include "Card.hpp"

class Moat : public Card{
    public:
        Moat() : Card("Moat", 2," +2 Cards. When another player plays an Attack card, you may reveal this from your hand, to be unaffected by it.", {CardType::ACTION, CardType::REACTION}) {}
        void onPlay(Game& g, Player& p) override;
        //void onReaction(Game& g, Player& p) override;
};