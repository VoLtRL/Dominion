#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Silver : public Card {
    public: 
        Silver() : Card("Silver", 3, "", {CardType::TREASURE}) {}
        void onPlay(Game& g, Player& p) const;
};