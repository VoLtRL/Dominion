#pragma once
#include "../abstract/Card.hpp"

class Copper : public Card {
    private:
        int value = 1;
    public: 
        Copper() : Card("Copper", 0, "", {CardType::TREASURE}) {}
        void onPlay(Game& g, Player& p);
};