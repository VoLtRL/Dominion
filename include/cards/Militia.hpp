#include "../abstract/Card.hpp"

class Militia : public Card {
    public:
        Militia() : Card("Militia", 4, "+2 Coins. Each other player discards down to 3 cards in hand.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p);
};