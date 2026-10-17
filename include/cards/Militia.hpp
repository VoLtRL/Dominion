#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Militia : public Card {
    public:
        Militia() : Card("Militia", 4, "+2 Coins. Each other player discards down to 3 cards in hand.", {CardType::ACTION, CardType::ATTACK }){};
        void onPlay(Game& g, Player& p) const;
        void onAttack(Game& g, Player& p, std::set<Player*> targets) const;
};