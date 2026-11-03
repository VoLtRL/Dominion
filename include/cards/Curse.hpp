#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Cursed.hpp"

class Curse : public Card, public Cursed {
    public: 
        Curse() : Card("Curse", 0, "-1 victory point", {CardType::CURSE}) {}
        int getCursePoints(const Player& p) const;
};