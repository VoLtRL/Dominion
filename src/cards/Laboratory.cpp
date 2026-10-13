#include "../../include/cards/Laboratory.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Laboratory::onPlay(Game& g, Player& p) const {
    std::cout << "You draw 2 cards and gain 1 action this turn!" << std::endl;
    // Draw 2 cards
    p.drawCardsToHand(2);
    // Add 1 action
    g.addPlayerActions(1);
}