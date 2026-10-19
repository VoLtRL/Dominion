#include "shared/cards/Duchy.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

int Duchy::getVictoryPoints(const Player& p) const {
    (void)p;
    return 3;
}