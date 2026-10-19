#include "shared/cards/Remodel.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Remodel::play(Game& g, Player& p) const {
    std::cout << "Trash a card from your hand and gain a card costing up to 2 more than it." << std::endl;
    std::vector<const Card*> selection = g.chooseCardsFromSelection(p.getHand().getCards(), 1, "trash", "exact", {CardType::ALL});
    if(!selection.empty()){
        std::vector<const Card*> gained = g.chooseCardsFromSelection(g.getBuyableCardsBelowCost(selection[0]->getCost()+2), 1, "gain", "exact", {CardType::ALL});
        if(!gained.empty()){
            p.addCardToDiscardStack(g.getCardFromCardDistrib(gained[0]->getName()));
        }
    }
}