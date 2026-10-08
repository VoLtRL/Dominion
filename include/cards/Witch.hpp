#pragma once
#include "../abstract/Card.hpp"


class Witch : public Card{
    public:
        Witch() : Card("Witch", 5," +2 Cards. Each other player gains a Curse.", {CardType::ACTION, CardType::ATTACK}) {}
        void onPlay(Game& g, Player& p);
};