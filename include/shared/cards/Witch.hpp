#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"
#include "shared/abstract/Attack.hpp"


class Witch : public Card, public Attack, public Action  {
    public:
        Witch() : Card("Witch", 5," +2 Cards. Each other player gains a Curse.", {CardType::ACTION, CardType::ATTACK}, Extension::DOMINION) {}
        void play(Game& g, Player& p) const;
        void attack(Game& g, Player& p, std::set<Player*> targets) const;
};