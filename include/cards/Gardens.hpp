#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"


class Gardens : public Card {
    public:
        Gardens() : Card("Gardens", 4, "Worth 1 Victory Point per 10 cards you have (rounded down).", {CardType::VICTORY}) {}
        int getVictoryPoints(const Player& p) const override;
};