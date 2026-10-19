#include "shared/cards/Province.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

int Province::getVictoryPoints(const Player& p) const {
    (void)p;
    return 6;
}