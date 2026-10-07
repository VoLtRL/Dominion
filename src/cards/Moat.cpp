#include "Moat.hpp"

Moat::onPlay(Game& g, Player& p) {
    // Draw 2 cards
    p.drawCards(2);
}

/*
Moat::onReaction(Game& g, Player& p) {
    // When another player plays an Attack card, you may reveal this from your hand to be unaffected by it.
    // This would require interaction with the game state to check for attacks and handle the reaction.
}
*/