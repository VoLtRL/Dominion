#pragma once 
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"
#include "../abstract/Attack.hpp"

class Thief : public Card, public Attack, public Action  {
    public:
        Thief() : Card("Thief", 4," Each other player reveals the top 2 cards of their deck, trashes a revealed Treasure you choose, and discards the rest. You may gain any trashed cards.", {CardType::ACTION, CardType::ATTACK}) {}
        void play(Game& g, Player& p) const;
        void attack(Game& g, Player& p, std::set<Player*> targets) const;
};