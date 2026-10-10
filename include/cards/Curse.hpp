#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"

class Curse : public Card {
    private:
        int vp = -1;
    public: 
        Curse() : Card("Curse", 0, "", {CardType::CURSE}) {}
        void onGain(Game& g, Player& p) const;
};