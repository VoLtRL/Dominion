#include "../../include/cards/Gold.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Gold::onPlay(Game& g, Player& p) const {
    g.addGold(value);
}