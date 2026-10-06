#pragma once
#include "Cards.hpp"

class Estate : public Card {
    private:
        int vp = 1;
    public: 
        Estate() : Card("Estate", 2, "", {CardType::Victory}) {}
        void onPlay(Game& g, Player& p) override;
};