#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Mine : public Card {
    public:
        Mine() : Card("Mine", 5, "Trash a Treasure card from your hand. Gain a Treasure card costing up to 3 more than it.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) const;
};