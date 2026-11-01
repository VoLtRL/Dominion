#include "shared/cards/Witch.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Witch::play(Game& g, Player& p) const {
    p.drawCardsToHand(2);
}

void Witch::attack(Game& g, Player& p, std::set<Player*> targets) const {

    for(Player* player : targets){
        if(player!=&p){
            if(g.isStackEmpty("Curse")){
                // Do nothing, no curses left
            }
            else{
                // Add a curse to the player's discard pile
                player->addCardToDiscardStack(g.getCardFromCardDistrib("Curse"));
            }
        }
    }
}