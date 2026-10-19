#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"

class ThroneRoom : public Card, public Action  {
    public:
        ThroneRoom() : Card("Throne Room", 4, "You may play an Action card from your hand twice.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};