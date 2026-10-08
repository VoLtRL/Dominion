#include "../abstract/Card.hpp"

class Chancellor : public Card {
    public:
        Chancellor() : Card("Chancellor", 3, "You may immediately put your deck into your discard pile. +2 Coins.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p);
};