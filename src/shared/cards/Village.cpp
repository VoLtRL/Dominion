#include "shared/cards/Village.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Village::play(Game& g, Player& p) const {
    std::cout << "You draw 1 card and gain 2 actions !" << std::endl;
    // Draw 1 card
    p.drawCardsToHand(1);
    // Add 2 actions
    g.addPlayerActions(2);
}