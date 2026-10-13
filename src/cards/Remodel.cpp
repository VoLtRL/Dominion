#include "../../include/cards/Remodel.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Remodel::onPlay(Game& g, Player& p) const {
    std::cout << "Trash a card from your hand and gain a card costing up to 2 more than it." << std::endl;
    const Card* c = g.chooseCardsFromSelection(p.getHand().getCards(), 1, "trash", "exact", {CardType::ALL})[0];
    const Card* gained = g.chooseCardsFromSelection(g.getBuyableCardsBelowCost(c->getCost()+2), 1, "gain", "exact", {CardType::ALL})[0];
    p.addCardToDiscardStack(gained);
    g.buyCard(gained);
}