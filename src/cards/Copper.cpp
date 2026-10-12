#include "../../include/cards/Copper.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Copper::onPlay(Game& g, Player& p) const {
    (void)p;
    g.addGold(1);
}