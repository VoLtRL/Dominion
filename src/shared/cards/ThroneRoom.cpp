#include "shared/cards/ThroneRoom.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void ThroneRoom::play(Game& g, Player& p) const {
    std::cout << "You can play an action from your hand twice!" << std::endl;
    std::vector<const Card*> actions;
    for(const Card* c : p.getHand().getCards()){
        if(c->isA(CardType::ACTION)){
            actions.push_back(c);
        }
    }

    if(actions.empty()){
        std::cout << "You don't have any action card..." << std::endl;
    }
    else{
        g.requestSelection(&p, actions, 1, SelectionMode::PLAY_CARD,
                           SelectionConstraint::EXACTLY,
                           Game::SelectionContext::THRONE_ROOM_PLAY);
    }
}