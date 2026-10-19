#include "shared/cards/Workshop.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Workshop::play(Game& g, Player& p) const {
    std::cout << "You get a card costing up to 4 coins!" << std::endl;
    std::vector<const Card*> cards = g.chooseCardsFromSelection(g.getBuyableCardsBelowCost(4), 1, "gain", "exact", {CardType::ALL});
    if(!cards.empty()){
        p.addCardToDiscardStack(g.getCardFromCardDistrib(cards[0]->getName()));
    }
}