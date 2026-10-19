#include "../../include/cards/Feast.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Feast::play(Game& g, Player& p) const {

    std::cout << "This card is trashed." << std::endl;
    p.removeCardFromHand(this);
    g.trashCard(this);

    std::cout << "You now gain a card costing up to 5 coins!" << std::endl;
    std::vector<const Card*> selection = g.getBuyableCardsBelowCost(5);
    if(!selection.empty()){
        selection = g.chooseCardsFromSelection(selection, 1, "gain", "exact", {CardType::ALL});
        if(!selection.empty()){
            p.addCardToDiscardStack(g.getCardFromCardDistrib(selection[0]->getName()));
        }
    }
}