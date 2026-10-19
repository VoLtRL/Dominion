#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Treasure.hpp"


class Gold : public Card, public Treasure {
    public: 
        Gold() : Card("Gold", 6, "", {CardType::TREASURE}) {}
        int getCoins() const;
};