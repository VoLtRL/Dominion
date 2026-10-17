#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Treasure.hpp"


class Silver : public Card, public Treasure {
    public: 
        Silver() : Card("Silver", 3, "", {CardType::TREASURE}) {}
        int getCoins() const;
};