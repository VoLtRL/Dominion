#include "Chapel.hpp"

Chapel::onPlay(Game& g, Player& p) {
    int cardsToTrash = std::min(4,p.getHand().size());
    // Prompt the player to select cards to trash
}