#pragma once
#include "Cards.hpp"

class Silver : public Card {
    private:
        int value = 2;
    public: 
        Silver() : Card("Silver", 3, "", {CardType::Treasure}) {}
        void onPlay(Game& g, Player& p) override;
};