#include "../abstract/Card.hpp"

class Smithy : public Card {
    public:
        Smithy() : Card("Smithy", 4, "+3 Cards.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p);
};