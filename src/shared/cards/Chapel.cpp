#include "shared/cards/Chapel.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"
#include <algorithm>

void Chapel::play(Game& g, Player& p) const {

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