#include "../../include/cards/Woodcutter.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Woodcutter::play(Game& g, Player& p) const {
    std::cout << "You gain 1 purchase and 2 coins this turn!" << std::endl;
    // Add 1 buy
    g.addPlayerPurchases(1);
    // Add 2 coins
    g.addGold(2);
}