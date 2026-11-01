#include "shared/cards/Woodcutter.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Woodcutter::play(Game& g, Player& p) const {
    // Add 1 buy
    g.addPlayerPurchases(1);
    // Add 2 coins
    g.addGold(2);
}