#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Silver : public Card {
    private:
        int value = 2;
    public: 
        Silver() : Card("Silver", 3, "", {CardType::TREASURE}) {}
        void onPlay(Game& g, Player& p) const;
};