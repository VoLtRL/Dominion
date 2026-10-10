#include "../../include/cards/Festival.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Festival::onPlay(Game& g, Player&) const {
    g.addPlayerActions(2);
    g.addPlayerPurchases(1);
    g.addGold(2);
}
