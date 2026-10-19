#include "shared/cards/Witch.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Witch::play(Game& g, Player& p) const {
    std::cout << "You draw 2 cards!" << std::endl;
    p.drawCardsToHand(2);
}

void Witch::attack(Game& g, Player& p, std::set<Player*> targets) const {
    std::cout << "Each other player gain a curse." << std::endl;

    for(Player* player : targets){
        if(player!=&p){
            if(g.isStackEmpty("Curse")){
                std::cout << "No curses left..." << std::endl;
            }
            else{
                std::cout << "Player " << player->getName() << " gets cursed!" << std::endl;
                p.addCardToDiscardStack(g.getCardFromCardDistrib("Curse"));
            }
        }
    }
}