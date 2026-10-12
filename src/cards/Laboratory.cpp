#include "../../include/cards/Laboratory.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Laboratory::onPlay(Game& g, Player& p) const {
    // Draw 2 cards
    p.drawCards(2);
    // Add 1 action
    g.addPlayerActions(1);
}