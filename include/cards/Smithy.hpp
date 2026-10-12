#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Smithy : public Card {
    public:
        Smithy() : Card("Smithy", 4, "+3 Cards.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) const;
};