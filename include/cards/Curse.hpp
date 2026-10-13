#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"

class Curse : public Card {
    public: 
        Curse() : Card("Curse", 0, "", {CardType::CURSE}) {}
        void onGain(Game& g, Player& p) const;
        void onTrash(Game& g, Player& p) const;
};