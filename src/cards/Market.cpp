#include "../../include/cards/Market.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Market::play(Game& g, Player& p) const {
    std::cout << "You draw 1 card and gain 1 action, 1 purchase and 1 coin this turn!" << std::endl;
    p.drawCardsToHand(1);
    g.addPlayerActions(1);
    g.addPlayerPurchases(1);
    g.addGold(1);
}