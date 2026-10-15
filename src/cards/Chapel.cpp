#include "../../include/cards/Chapel.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"
#include <algorithm>

void Chapel::onPlay(Game& g, Player& p) const {

    std::cout << "You may trash up to 4 cards from your hand." << std::endl;
    auto selection = g.chooseCardsFromSelection(p.getHand().getCards(), p.getHand().getSize(), "trash", "most", {CardType::ALL});
    if(!selection.empty()){
        std::cout << "Trashing the following cards : " << std::endl;
        g.displaySelection(selection);
        for(auto card : selection){
            p.removeCardFromHand(card);
            g.trashCard(card);
        }
    }

}