#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Duchy : public Card {
    private:
        int vp = 3;
    public: 
        Duchy() : Card("Duchy", 5, "", {CardType::VICTORY}) {}
        void onPlay(Game& g, Player& p) const;
};