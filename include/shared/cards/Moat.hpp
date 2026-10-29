#pragma once
#include "shared/abstract/Card.hpp"
#include "shared/abstract/CardType.hpp"
#include "shared/abstract/Reaction.hpp"
#include "shared/abstract/Action.hpp"


class Moat : public Card, public Reaction, public Action {
    public:
        Moat() : Card("Moat", 2," +2 Cards. When another player plays an Attack card, you may reveal this from your hand, to be unaffected by it.", {CardType::ACTION, CardType::REACTION}, Extension::DOMINION) {
            setReactionTypes({EventType::ATTACK_PLAYED_OTHER});
        }
        void play(Game& g, Player& p) const;
        void react(EventType type, Game& game, Player* owner, Player* actor) const ;
};