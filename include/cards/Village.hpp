#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Village : public Card{
    public:
        Village() : Card("Village", 3," +1 Card; +2 Actions.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p) const;
};