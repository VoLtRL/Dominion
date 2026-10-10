#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Festival : public Card {
    public:
        Festival() : Card("Festival", 5, "+2 Actions, +1 Buy, +2 Coins.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) const;
};