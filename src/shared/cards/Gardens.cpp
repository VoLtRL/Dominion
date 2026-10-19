#include "shared/gameplay/Game.hpp"
#include "shared/cards/Gardens.hpp"
#include "shared/player/Player.hpp"

int Gardens::getVictoryPoints(const Player& p) const {
    int totalCards = p.getTotalCardCount();
    return totalCards / 10;
}