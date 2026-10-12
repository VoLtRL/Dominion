#include "../include/cards/Province.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Province::onGain(Game& g, Player& p) const {
    p.addVictoryPoints(vp);
}