#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"

class Smithy : public Card, public Action  {
    public:
        Smithy() : Card("Smithy", 4, "+3 Cards.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};