#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"
#include "shared/abstract/Attack.hpp"

class Spy : public Card, public Attack, public Action  {
    public:
        Spy() : Card("Spy", 4, "Each player (including you) reveals the top card of their deck and either discards it or puts it back, your choice.", {CardType::ACTION, CardType::ATTACK}, Extension::DOMINION_REMOVED_CARDS) {}
        void play(Game& g, Player& p) const;
        void attack(Game& g, Player& p, std::set<Player*> targets) const;
};