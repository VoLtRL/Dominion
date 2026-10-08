#include "../abstract/Card.hpp"

class Market : public Card {
    public:
        Market() : Card("Market", 5, "+1 Card, +1 Action, +1 Buy, +1 Coin.", {CardType::ACTION}){}
        void onPlay(Game& g, Player& p);
};