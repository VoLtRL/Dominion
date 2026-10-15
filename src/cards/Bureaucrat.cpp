#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Bureaucrat.hpp"
#include "../../include/player/Player.hpp"

void Bureaucrat::onPlay(Game &g, Player &p) const
{
    std::cout << "A silver is added to your draw pile !" << std::endl;
    const Card* silver = g.getCardFromCardDistrib("Silver");
    if (silver == nullptr){
        std::cout << "There aren't any silver left..." << std::endl;
    }
    else{
        p.addCardToDrawStack(silver);
    }
    
    std::cout << "Every opponents must show and put on their draw pile a victory card from their hand." << std::endl;
    for (Player player : g.getPlayers()){
        if(p != player){
            std::cout << "Player " << player.getName() << " : show a victory card in your hand to put on top of your draw pile." << std::endl;
            std::vector<const Card *> selection = g.chooseCardsFromSelection(player.getHand().getCards(), 1, "display and put on your draw pile", "exact", {CardType::VICTORY});
            if(!selection.empty()){
                std::cout << "Putting the following card on top of your draw pile : " << std::endl;
                g.displaySelection(selection);
                player.removeCardFromHand(selection[0]);
                player.addCardToDrawStack(selection[0]);
            }
        }
    }
}
