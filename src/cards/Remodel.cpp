#include "../../include/cards/Remodel.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Remodel::play(Game& g, Player& p) const {
    std::cout << "Trash a card from your hand and gain a card costing up to 2 more than it." << std::endl;
    SelectionParameters params = InputManager::createSelectionParameters(1, "Trash one card", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::ALL}, p.getHand().getCards());
    std::vector<const Card*> selection = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);
    if(!selection.empty()){
        SelectionParameters params = InputManager::createSelectionParameters(1, "Gain one card", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::ALL}, g.getBuyableCardsBelowCost(selection[0]->getCost()+2));
        std::vector<const Card*> gained = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);
        if(!gained.empty()){
            p.addCardToDiscardStack(g.getCardFromCardDistrib(gained[0]->getName()));
        }
    }
}