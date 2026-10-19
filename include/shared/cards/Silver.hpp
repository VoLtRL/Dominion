#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Treasure.hpp"


class Silver : public Card, public Treasure {
    public: 
        Silver() : Card("Silver", 3, "", {CardType::TREASURE}) {}
        int getCoins() const;
};