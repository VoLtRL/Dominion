#pragma once
#include "../abstract/Card.hpp"


class Silver : public Card {
    private:
        int value = 2;
    public: 
        Silver() : Card("Silver", 3, "", {CardType::TREASURE}) {}
        void onPlay(Game& g, Player& p);
};