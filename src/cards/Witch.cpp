#include "../../include/cards/Witch.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Witch::onPlay(Game& g, Player& p) const {
    std::cout << "You draw 2 cards!" << std::endl;
    p.drawCardsToHand(2);
    std::cout << "Each other player gain a curse." << std::endl;

    for(Player player : g.getPlayers()){
        if(player!=p){
            if(g.isStackEmpty("Curse")){
                std::cout << "No curses left..." << std::endl;
            }
            else{
                std::cout << "Player " << player.getName() << " gets cursed!" << std::endl;
                const Card* curse = Factory::get("Curse");
                p.addCardToDiscardStack(curse);
                g.buyCard(curse);
            }
        }
    }
}