#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Laboratory : public Card{
    public:
        Laboratory() : Card("Laboratory", 5," +2 Cards; +1 Action.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) const;
};