#include "../../include/cards/Workshop.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Workshop::onPlay(Game& g, Player& p) const {
    std::cout << "You get a card costing up to 4 coins!" << std::endl;
    const Card* c = g.chooseCardsFromSelection(g.getBuyableCardsBelowCost(4), 1, "gain", "exact", {CardType::ALL})[0];
    p.addCardToDiscardStack(c);
    g.buyCard(c);
}