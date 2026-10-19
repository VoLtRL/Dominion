#include "../../include/cards/ThroneRoom.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

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
        std::vector<const Card*> toPlay = g.chooseCardsFromSelection(actions, 1, "play twice", "exact", {CardType::ACTION});
        if (toPlay.empty()){
            return;
        }
        const Action* action = dynamic_cast<const Action*>(toPlay[0]);
        action->play(g, p);
        std::cout << "One more time !" << std::endl;
        g.playAction(p, toPlay[0]);
    }
}