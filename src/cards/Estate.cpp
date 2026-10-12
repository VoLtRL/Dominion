#include "../../include/cards/Estate.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Estate::onGain(Game& g, Player& p) const{
    p.addVictoryPoints(vp);
}