#include "../../include/cards/Mine.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Mine::onPlay(Game& g, Player& p) const {
    std::cout <<"Trash a treasure from your hand and gain one costing up to 3 coins more into your hand!" << std::endl;
    std::vector<const Card*> treasures;
    for(const Card* c : p.getHand().getCards()){
        if(c->isA(CardType::TREASURE)){
            treasures.push_back(c);
        }
    }
    if(treasures.empty()){
        std::cout << "You don't have any treasures..." << std::endl;
    }
    else{
        treasures = g.chooseCardsFromSelection(treasures, 1, "trash", "most", {CardType::TREASURE});
        if(!treasures.empty()){
            std::vector<const Card*> validCards = g.getBuyableCardsBelowCost(treasures[0]->getCost()+3);
            if(!validCards.empty()){
                validCards = g.chooseCardsFromSelection(validCards, 1, "gain", "exact", {CardType::ALL});
                p.addCardToHand(validCards[0]);
                g.buyCard(validCards[0]);
            }
        }
    }
}