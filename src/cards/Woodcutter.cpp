#include "Woodcutter.hpp"

Woodcutter::onPlay(Game& g, Player& p) {
    // Add 1 buy
    p.addBuys(1);
    // Add 2 coins
    p.addCoins(2);
}