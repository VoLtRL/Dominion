#pragma once
#include "../abstract/Card.hpp"

class Estate : public Card {
    private:
        int vp = 1;
    public: 
        Estate() : Card("Estate", 2, "", {CardType::VICTORY}) {}
        void onPlay(Game& g, Player& p);
};