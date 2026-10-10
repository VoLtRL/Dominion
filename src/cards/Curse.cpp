#include "../../include/cards/Curse.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Curse::onGain(Game& g, Player& p) const {
    (void)g;
    p.addVictoryPoints(vp);
}