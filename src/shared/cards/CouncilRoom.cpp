#include "shared/cards/CouncilRoom.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void CouncilRoom::play(Game& g, Player& p) const {
    std::cout << "You draw 4 cards and can buy one more card!" << std::endl;
    p.drawCardsToHand(4);
    g.addPlayerPurchases(1);
    std::cout << "Every other player draws one card." << std::endl;
    for(auto player : g.getPlayers()){
        if(p!=player){
            player.drawCardToHand();
        }
    }
}