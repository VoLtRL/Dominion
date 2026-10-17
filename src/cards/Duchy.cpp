#include "../../include/cards/Duchy.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

int Duchy::getVictoryPoints(const Player& p) const {
    (void)p;
    return 3;
}