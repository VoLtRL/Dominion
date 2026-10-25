#include "../../include/cards/Chapel.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"
#include <algorithm>

void Chapel::play(Game& g, Player& p) const {

    std::cout << "You may trash up to 4 cards from your hand." << std::endl;
    SelectionParameters params = InputManager::createSelectionParameters(4, "Trash up to 4 cards", g.HUDFormat(), SelectionMode::AT_MOST, {CardType::ALL}, p.getHand().getCards());
    std::vector<const Card*> selection = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);
    if(!selection.empty()){
        std::cout << "Trashing the following cards : " << std::endl;
        InputManager::displaySelection(selection);
        for(auto card : selection){
            p.removeCardFromHand(card);
            g.trashCard(card);
        }
    }

}