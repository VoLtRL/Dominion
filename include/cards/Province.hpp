#pragma once
#include "../abstract/Card.hpp"


class Province : public Card {
    private:
        int vp = 6;
    public: 
        Province() : Card("Province", 8, "", {CardType::VICTORY}) {}
        void onPlay(Game& g, Player& p);
};