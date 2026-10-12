#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"

class Copper : public Card {
    private:
        int value = 1;
    public: 
        Copper() : Card("Copper", 0, "", {CardType::TREASURE}) {}
        void onPlay(Game& g, Player& p) const;
};