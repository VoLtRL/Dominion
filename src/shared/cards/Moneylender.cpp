#include "shared/cards/Moneylender.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Moneylender::play(Game& g, Player& p) const {
    std::cout << "You may trash a Copper from your hand to gain 3 coins this turn." << std::endl;
    std::vector<const Card*> coppers;
    for(const Card* c : p.getHand().getCards()){
        if(c->getName() == "Copper"){
            coppers.push_back(c);
        }
    }
    if(coppers.empty()){
        std::cout << "You don't have any copper in your hand." << std::endl;
    } else {
        g.requestSelection(&p, coppers, 1, SelectionMode::TRASH,
                           SelectionConstraint::AT_MOST,
                           Game::SelectionContext::MONEYLENDER_TRASH);
    }
}