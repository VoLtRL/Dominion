#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/Attack.hpp"
#include "shared/abstract/Action.hpp"

class Bureaucrat : public Card, public Attack, public Action  {
    public:
        Bureaucrat() : Card("Bureaucrat", 4, "Gain a Silver card; put it on top of your deck. Each other player reveals a Victory card from their hand and puts it on their deck (or reveals a hand with no Victory cards).", {CardType::ACTION, CardType::ATTACK}, Extension::DOMINION) {}
        void play(Game& g, Player& p) const;
        void attack(Game& g, Player& p, std::set<Player*> targets) const;
};