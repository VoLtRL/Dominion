#include "shared/cards/Estate.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

int Estate::getVictoryPoints(const Player& p) const {
    (void)p;
    return 1;
}