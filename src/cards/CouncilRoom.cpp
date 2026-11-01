#include "../../include/cards/CouncilRoom.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void CouncilRoom::play(Game& g, Player& p) const {
    std::cout << "You draw 4 cards and can buy one more card!" << std::endl;
    p.drawCardsToHand(4);
    g.addPlayerPurchases(1);
    std::cout << "Every other player draws one card." << std::endl;
    for(const std::unique_ptr<Player>& player : g.getPlayers()){
        if(&p!=player.get()){
            player->drawCardToHand();
        }
    }
}