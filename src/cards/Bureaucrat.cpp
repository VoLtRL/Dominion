#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Bureaucrat.hpp"
#include "../../include/player/Player.hpp"

void Bureaucrat::onPlay(Game &g, Player &p) const
{
    std::cout << "A silver is added to your draw pile !" << std::endl;
    if(g.isStackEmpty("Silver")){
        std::cout << "There aren't any silver left..." << std::endl;
    }
    else{
        const Card* silver = Factory::get("Silver");
        p.addCardToDrawStack(silver);
        g.buyCard(silver);
    }
    
    std::cout << "Every opponents must show and put on their draw pile a victory card from their hand." << std::endl;
    for (Player player : g.getPlayers()){
        if(p != player){
            std::cout << "Player " << player.getName() << " : show a victory card in your hand to put on top of your draw pile." << std::endl;
            std::vector<const Card*> cards = player.getHand().getCards();
            std::vector<const Card*> validCards;
            for(const Card* c : cards){
                if(c->isA(CardType::VICTORY)){
                    validCards.push_back(c);
                }
            }
            if(validCards.empty()){
                std::cout << "No victory card, showing hand : " <<std::endl;
                g.displaySelection(cards);
            }
            else{
                std::vector<const Card *> selection = g.chooseCardsFromSelection(validCards, 1, "display and put on your draw pile", "exact", {CardType::VICTORY});
                std::cout << "Putting the following card on top of your draw pile : " << std::endl;
                g.displaySelection(selection);
                player.removeCardFromHand(selection[0]);
                player.addCardToDrawStack(selection[0]);
            }
        }
    }
}
