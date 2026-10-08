#include "../abstract/Card.hpp"

class Adventurer : public Card {
    public:
        Adventurer() : Card("Adventurer", 6, "Reveal cards from your deck until you reveal 2 Treasure cards. Put those Treasure cards into your hand and discard the other revealed cards.", {CardType::ACTION}) {}
        void onPlay(Game& g, Player& p);
};