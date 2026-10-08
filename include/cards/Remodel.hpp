#include "../abstract/Card.hpp"

class Remodel : public Card {
    public:
        Remodel() : Card("Remodel", 4, "Trash a card from your hand. Gain a card costing up to 2 more than it.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p);
};