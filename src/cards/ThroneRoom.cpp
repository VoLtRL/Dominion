#include "../../include/cards/ThroneRoom.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void ThroneRoom::onPlay(Game& g, Player& p) const {
    g.addPlayerActions(1);
    (void)p;
}