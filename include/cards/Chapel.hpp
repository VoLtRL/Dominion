#pragma once
#include "Card.hpp"

class Chapel : public Card{
    public:
        Chapel() : Card("Chapel", 2," Trash up to 4 cards from your hand.", {CardType::Action}) {}
        void onPlay(Game& g, Player& p) override;
}