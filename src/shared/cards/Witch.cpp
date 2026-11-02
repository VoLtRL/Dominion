#include "shared/cards/Witch.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Witch::play(Game& g, Player& p) const {
    p.drawCardsToHand(2);
}

void Witch::attack(Game& g, Player& p, std::set<Player*> targets) const {

    for(Player* player : targets){
        if(player==&p) continue;
        const Card* curseCard = g.getCardFromCardDistrib("Curse");
        if(curseCard != nullptr) player->addCardToDiscardStack(curseCard);
    }
}