#pragma once
#include "Card.hpp"

class Gold : public Card {
    private:
        int value = 3;
    public: 
        Gold() : Card("Gold", 6, "", {CardType::TREASURE}) {}
        void onPlay(Game& g, Player& p) override;
};