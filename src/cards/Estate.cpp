#include "../../include/cards/Estate.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

int Estate::getVictoryPoints(const Player& p) const {
    (void)p;
    return 1;
}