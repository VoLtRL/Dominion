#include "../../include/cards/CouncilRoom.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void CouncilRoom::onPlay(Game& g, Player& p) const {
    p.drawCards(4);
    g.addPlayerPurchases(1);
}