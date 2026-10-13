#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Duchy : public Card {
    public: 
        Duchy() : Card("Duchy", 5, "", {CardType::VICTORY}) {}
        void onGain(Game& g, Player& p) const;
        void onTrash(Game& g, Player& p) const;
};