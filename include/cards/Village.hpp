#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Action.hpp"


class Village : public Card, public Action {
    public:
        Village() : Card("Village", 3," +1 Card; +2 Actions.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};