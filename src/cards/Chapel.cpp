#include "../../include/cards/Chapel.hpp"
#include <algorithm>

void Chapel::onPlay(Game& g, Player& p) {
    int cardsToTrash = std::min(4,static_cast<int>(p.getHand().getSize()));
    // Prompt the player to select cards to trash
}