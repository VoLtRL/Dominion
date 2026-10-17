#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"
#include "../abstract/Attack.hpp"


class Witch : public Card, public Attack, public Action  {
    public:
        Witch() : Card("Witch", 5," +2 Cards. Each other player gains a Curse.", {CardType::ACTION, CardType::ATTACK}) {}
        void play(Game& g, Player& p) const;
        void attack(Game& g, Player& p, std::set<Player*> targets) const;
};