#pragma once
#include "Cards.hpp"

class Duchy : public Card {
    private:
        int vp = 3;
    public: 
        Duchy() : Card("Duchy", 5, "", {CardType::Victory}) {}
        void onPlay(Game& g, Player& p) override;
};