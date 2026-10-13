#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Gold : public Card {
    public: 
        Gold() : Card("Gold", 6, "", {CardType::TREASURE}) {}
        void onPlay(Game& g, Player& p) const;
};