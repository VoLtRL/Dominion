#include "../../include/cards/Village.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Village::onPlay(Game& g, Player& p) const {
    std::cout << "You draw 1 card and gain 2 actions !" << std::endl;
    // Draw 1 card
    p.drawCardsToHand(1);
    // Add 2 actions
    g.addPlayerActions(2);
}