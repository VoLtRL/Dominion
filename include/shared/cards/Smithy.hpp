#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"

class Smithy : public Card, public Action  {
    public:
        Smithy() : Card("Smithy", 4, "+3 Cards.", {CardType::ACTION}, Extension::DOMINION) {}
        void play(Game& g, Player& p) const;
};