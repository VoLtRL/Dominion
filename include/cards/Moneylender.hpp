#include "../abstract/Card.hpp"

class Moneylender : public Card {
    public:
        Moneylender() : Card("Moneylender", 4, "Trash a Copper card from your hand. +3 Coins.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p);
};