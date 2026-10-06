#pragma once
#include "Cards.hpp"

class Province : public Card {
    private:
        int vp = 6;
    public: 
        Province() : Card("Province", 8, "", {CardType::Victory}) {}
        void onPlay(Game& g, Player& p) override;
};