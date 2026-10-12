#include "../../include/cards/Witch.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Witch::onPlay(Game& g, Player& p) const {
    // Draw 2 cards
    p.drawCards(2);
    // Each other player gains a Curse
}