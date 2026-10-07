#pragma once
#include "Card.hpp"

class Curse : public Card {
    private:
        int vp = -1;
    public: 
        Curse() : Card("Curse", 0, "", {CardType::Curse}) {}
        void onPlay(Game& g, Player& p) override;
};