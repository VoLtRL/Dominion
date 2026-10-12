#pragma once
#include "../abstract/Card.hpp"

class CouncilRoom : public Card {
    public:
        CouncilRoom() : Card("Council Room", 5, "+4 Cards. +1 Buy. Each other player draws a card.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) const;
};