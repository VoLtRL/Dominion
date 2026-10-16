#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Witch : public Card{
    public:
        Witch() : Card("Witch", 5," +2 Cards. Each other player gains a Curse.", {CardType::ACTION, CardType::ATTACK}) {}
        void onPlay(Game& g, Player& p) const;
        void onAttack(Game& g, Player& p, std::set<Player*> targets) const;
};