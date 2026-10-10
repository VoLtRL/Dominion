#include "../../include/cards/Copper.hpp"

void Copper::onPlay(Game& g, Player& p) {
    g.addGold(1);
}