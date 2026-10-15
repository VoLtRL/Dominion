#include "../../include/cards/Province.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

int Province::getVictoryPoints(const Player& p) const {
    (void)p;
    return 6;
}