#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Gardens.hpp"
#include "../../include/player/Player.hpp"

int Gardens::getVictoryPoints(const Player& p) const {
    int totalCards = p.getTotalCardCount();
    return totalCards / 10;
}