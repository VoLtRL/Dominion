#include "../../include/cards/Mine.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Mine::play(Game& g, Player& p) const {
    std::cout <<"Trash a treasure from your hand and gain one costing up to 3 coins more into your hand!" << std::endl;
    std::vector<const Card*> treasures = g.chooseCardsFromSelection(p.getHand().getCards(), 1, "trash", "most", {CardType::TREASURE});
    if(!treasures.empty()){
        std::vector<const Card*> validCards = g.getBuyableCardsBelowCost(treasures[0]->getCost()+3);
        if(!validCards.empty()){
            validCards = g.chooseCardsFromSelection(validCards, 1, "gain", "exact", {CardType::TREASURE});
            if(!validCards.empty()){
                p.addCardToDiscardStack(g.getCardFromCardDistrib(validCards[0]->getName()));
            }
        }
    }  
}
