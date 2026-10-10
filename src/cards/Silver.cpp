#include "../../include/cards/Silver.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Silver::onPlay(Game& g, Player& p) const {
    g.addGold(value);
}