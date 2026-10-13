#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Estate : public Card {
    public: 
        Estate() : Card("Estate", 2, "", {CardType::VICTORY}) {}
        void onGain(Game& g, Player& p) const;
        void onTrash(Game& g, Player& p) const;
};