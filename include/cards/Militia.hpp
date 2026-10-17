#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"
#include "../abstract/Attack.hpp"

class Militia : public Card, public Action, public Attack {
    public:
        Militia() : Card("Militia", 4, "+2 Coins. Each other player discards down to 3 cards in hand.", {CardType::ACTION, CardType::ATTACK }){};
        void play(Game& g, Player& p) const;
        void attack(Game& g, Player& p, std::set<Player*> targets) const;
};