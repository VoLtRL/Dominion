#include "../../include/cards/ThroneRoom.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void ThroneRoom::play(Game& g, Player& p) const {
    std::cout << "You can play an action from your hand twice!" << std::endl;

    SelectionParameters params = InputManager::createSelectionParameters(1, "Play one action twice", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::ACTION}, p.getHand().getCards());
    std::vector<const Card*> toPlay = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);
    if (toPlay.empty()){
        return;
    }
    const Action* action = dynamic_cast<const Action*>(toPlay[0]);
    action->play(g, p);
    std::cout << "One more time !" << std::endl;
    g.playAction(p, toPlay[0]);
}