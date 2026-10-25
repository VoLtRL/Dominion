#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Bureaucrat.hpp"
#include "../../include/player/Player.hpp"

void Bureaucrat::play(Game &g, Player &p) const
{
    std::cout << "A silver is added to your draw pile !" << std::endl;
    const Card* silver = g.getCardFromCardDistrib("Silver");
    if (silver == nullptr){
        std::cout << "There aren't any silver left..." << std::endl;
    }
    else{
        p.addCardToDrawStack(silver);
    }
    
}

void Bureaucrat::attack(Game& g, Player& p, std::set<Player*> targets) const{
    std::cout << "Every opponents must show and put on their draw pile a victory card from their hand." << std::endl;
    for (Player* player : targets){
        std::cout << "Player " << player->getName() << " : show a victory card in your hand to put on top of your draw pile." << std::endl;
        SelectionParameters params = InputManager::createSelectionParameters(1, "Display and put one card on your draw pile", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::VICTORY},  player->getHand().getCards());
        std::vector<const Card *> selection = InputManager::chooseCardsFromSelection(*player, g.getCurrentPhase(), params);
        if(!selection.empty()){
            std::cout << "Putting the following card on top of your draw pile : " << std::endl;
            InputManager::displaySelection(selection);
            player->removeCardFromHand(selection[0]);
            player->addCardToDrawStack(selection[0]);
        }
    }
}
