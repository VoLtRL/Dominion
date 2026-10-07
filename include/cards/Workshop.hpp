#pragma once
#include "Card.hpp"

class Workshop : public Card{
    public:
        Workshop() : Card("Workshop", 3," Gain a card costing up to 4.", {CardType::Action}) {}
        void onPlay(Game& g, Player& p) override;
};