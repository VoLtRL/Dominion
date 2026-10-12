#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Province : public Card {
    private:
        int vp = 6;
    public: 
        Province() : Card("Province", 8, "", {CardType::VICTORY}) {}
        void onGain(Game& g, Player& p) const;
};