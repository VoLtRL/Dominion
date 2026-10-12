#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Woodcutter : public Card{
    public:
        Woodcutter() : Card("Woodcutter", 3," +1 Buy; +2 Coins.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) const;
};