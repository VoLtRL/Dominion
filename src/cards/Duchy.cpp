#include "../../include/cards/Duchy.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Duchy::onPlay(Game& g, Player& p) const {
    p.addVictoryPoints(vp);
}