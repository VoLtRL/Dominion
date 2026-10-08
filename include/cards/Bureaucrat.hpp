#include "../abstract/Card.hpp"

class Bureaucrat : public Card {
    public:
        Bureaucrat() : Card("Bureaucrat", 4, "Gain a Silver card; put it on top of your deck. Each other player reveals a Victory card from their hand and puts it on their deck (or reveals a hand with no Victory cards).", {CardType::ACTION, CardType::ATTACK}) {}
        void onPlay(Game& g, Player& p);
};