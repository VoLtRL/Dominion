#pragma once
#include "../abstract/Card.hpp"

class Gardens : public Card {
    public:
        Gardens() : Card("Gardens", 4, "Worth 1 Victory Point per 10 cards you have (rounded down).", {CardType::VICTORY}) {}
        void onGain(Game& g, Player& p);
};