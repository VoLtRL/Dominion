#pragma once
#include "../abstract/Card.hpp"
#include "../abstract/CardType.hpp"
#include "../abstract/Reaction.hpp"
#include "../abstract/Action.hpp"


class Moat : public Card, public Reaction, public Action {
    public:
        Moat() : Card("Moat", 2," +2 Cards. When another player plays an Attack card, you may reveal this from your hand, to be unaffected by it.", {CardType::ACTION, CardType::REACTION}) {}
        void play(Game& g, Player& p) const;
        bool canReact(GameEvent* event, Game* game, Player* owner) const;
        void react(GameEvent* event, Game* game, Player* owner) const ;
};