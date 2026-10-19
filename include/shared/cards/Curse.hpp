#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Cursed.hpp"

class Curse : public Card, public Cursed {
    public: 
        Curse() : Card("Curse", 0, "", {CardType::CURSE}) {}
        int getCursePoints(const Player& p) const;
};