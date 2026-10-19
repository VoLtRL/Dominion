#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"

class Library : public Card, public Action  {
    public:
        Library() : Card("Library", 5, "Draw until you have 7 cards in hand, skipping any Action cards you choose to set aside.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};