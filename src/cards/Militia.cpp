#include "../../include/cards/Militia.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Militia::onPlay(Game& g, Player&) const {
    g.addGold(2);
}