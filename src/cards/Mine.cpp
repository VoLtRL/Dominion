#include "../../include/cards/Mine.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Mine::play(Game& g, Player& p) const {
    std::cout <<"Trash a treasure from your hand and gain one costing up to 3 coins more into your hand!" << std::endl;
    SelectionParameters params = InputManager::createSelectionParameters(1, "Trash one treasure", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::TREASURE}, p.getHand().getCards());
    std::vector<const Card*> treasures = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);
    if(!treasures.empty()){
        std::vector<const Card*> validCards = g.getBuyableCardsBelowCost(treasures[0]->getCost()+3);
        if(!validCards.empty()){
            SelectionParameters params = InputManager::createSelectionParameters(1, "Gain one treasure", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::TREASURE},  validCards);
            validCards = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);
            if(!validCards.empty()){
                p.addCardToDiscardStack(g.getCardFromCardDistrib(validCards[0]->getName()));
            }
        }
    }  
}
