#include "../../include/cards/Festival.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Festival::onPlay(Game& g, Player&) const {

    std::cout << "You gain 2 actions, 1 purchase and 2 coins this turn!" << std::endl;
    g.addPlayerActions(2);
    g.addPlayerPurchases(1);
    g.addGold(2);
}
