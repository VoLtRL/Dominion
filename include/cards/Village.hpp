#pragma once
#include "Card.hpp"

class Village : public Card{
    public:
        Village() : Card("Village", 3," +1 Card; +2 Actions.", {CardType::Action}) {}
        void onPlay(Game& g, Player& p) override;
};