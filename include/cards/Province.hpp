#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"



class Province : public Card {
    public: 
        Province() : Card("Province", 8, "", {CardType::VICTORY}) {}
        void onGain(Game& g, Player& p) const;
        void onTrash(Game& g, Player& p) const;
};