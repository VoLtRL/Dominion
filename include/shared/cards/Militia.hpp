#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"
#include "shared/abstract/Attack.hpp"

class Militia : public Card, public Action, public Attack {
    public:
        Militia() : Card("Militia", 4, "+2 Coins. Each other player discards down to 3 cards in hand.", {CardType::ACTION, CardType::ATTACK }, Extension::DOMINION){};
        void play(Game& g, Player& p) const;
        void attack(Game& g, Player& p, std::set<Player*> targets) const;
};