#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Treasure.hpp"


class Gold : public Card, public Treasure {
    public: 
        Gold() : Card("Gold", 6, "", {CardType::TREASURE}) {}
        int getCoins() const;
};