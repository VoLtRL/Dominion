#include "../../include/cards/Village.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Village::onPlay(Game& g, Player& p) const {
    // Draw 1 card
    p.drawCards(1);
    // Add 2 actions
    g.addPlayerActions(2);
}