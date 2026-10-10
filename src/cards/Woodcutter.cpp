#include "../../include/cards/Woodcutter.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Woodcutter::onPlay(Game& g, Player& p) const {
    // Add 1 buy
    g.addPlayerPurchases(1);
    // Add 2 coins
    g.addGold(2);
}