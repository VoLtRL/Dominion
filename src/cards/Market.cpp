#include "../../include/cards/Market.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Market::onPlay(Game& g, Player& p) const {
    p.drawCards(1);
    g.addPlayerActions(1);
    g.addPlayerPurchases(1);
    g.addGold(1);
}