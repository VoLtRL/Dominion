#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class ThroneRoom : public Card {
    public:
        ThroneRoom() : Card("Throne Room", 4, "You may play an Action card from your hand twice.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) const;
};