#include "Village.hpp"

Village::onPlay(Game& g, Player& p) {
    // Draw 1 card
    p.drawCards(1);
    // Add 2 actions
    p.addActions(2);
}