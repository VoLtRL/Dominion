#pragma once
#include "Cards.hpp"

class Copper : public Card {
    private:
        int value = 1;
    public: 
        Copper() : Card("Copper", 0, "", {CardType::Treasure}) {}
        void onPlay(Game& g, Player& p) override;
};