#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Action.hpp"


class Village : public Card, public Action {
    public:
        Village() : Card("Village", 3," +1 Card; +2 Actions.", {CardType::ACTION}) {}
        void play(Game& g, Player& p) const;
};