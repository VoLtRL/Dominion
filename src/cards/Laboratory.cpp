#include "Laboratory.hpp"

Laboratory::onPlay(Game& g, Player& p) {
    // Draw 2 cards
    p.drawCards(2);
    // Add 1 action
    p.addActions(1);
}