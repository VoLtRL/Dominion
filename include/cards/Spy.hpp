#include "../abstract/Card.hpp"

class Spy : public Card {
    public:
        Spy() : Card("Spy", 4, "Each player (including you) reveals the top card of their deck and either discards it or puts it back, your choice.", {CardType::ACTION, CardType::ATTACK}) {}
        void onPlay(Game& g, Player& p);
};