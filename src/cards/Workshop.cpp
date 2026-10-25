#include "../../include/cards/Workshop.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Workshop::play(Game& g, Player& p) const {
    std::cout << "You get a card costing up to 4 coins!" << std::endl;
    SelectionParameters params = InputManager::createSelectionParameters(1, "Gain a card", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::ALL}, g.getBuyableCardsBelowCost(4));
    std::vector<const Card*> cards = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);     
    if(!cards.empty()){
        p.addCardToDiscardStack(g.getCardFromCardDistrib(cards[0]->getName()));
    }
}