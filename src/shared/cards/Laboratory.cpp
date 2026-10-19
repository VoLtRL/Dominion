#include "shared/cards/Laboratory.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Laboratory::play(Game& g, Player& p) const {
    std::cout << "You draw 2 cards and gain 1 action this turn!" << std::endl;
    // Draw 2 cards
    p.drawCardsToHand(2);
    // Add 1 action
    g.addPlayerActions(1);
}