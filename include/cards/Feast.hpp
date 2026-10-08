#pragma once
#include "Card.hpp"

class Feast : public Card{
    public:
        Feast() : Card("Feast", 4," Trash this card. Gain a card costing up to 5.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) override;
};