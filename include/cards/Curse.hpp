#pragma once
#include "../abstract/Card.hpp"

class Curse : public Card {
    private:
        int vp = -1;
    public: 
        Curse() : Card("Curse", 0, "", {CardType::CURSE}) {}
        void onPlay(Game& g, Player& p);
};