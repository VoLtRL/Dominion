#include "../../include/cards/Thief.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

#include <algorithm>

void Thief::play(Game& g, Player& p) const {
    
}

void Thief::attack(Game& g, Player& p, std::set<Player*> targets) const {
    std::cout << "Each other player reveals the top 2 cards of their deck. If there are Treasure Cards, you choose one of them to trash. You may gain any or all trashed Treasures, and the non-treasure cards revealed are discarded." << std::endl;
    std::vector<const Card*> treasures;
    for(Player* player : targets){
        if(player!=&p){
            std::vector<const Card*> revealed = {player->drawCard(), player->drawCard()};
            std::cout << "Revealed cards : " << std::endl;
            InputManager::displaySelection(revealed);

            std::vector<const Card*> localTreasures;
            for(const Card* c : revealed){
                if(c->isA(CardType::TREASURE)){
                    localTreasures.push_back(c);
                }
            }
            const Card* selected = nullptr;
            switch(localTreasures.size()){
                case 0:
                    std::cout << "No treasures..." << std::endl;
                    break;
                case 1:
                    std::cout << "Only one treasure, this card is automatically chosen." << std::endl;
                    selected = localTreasures[0];
                    break;
                default:
                    std::cout << "Player " << p.getName() << ", choose the one you want to trash !" << std::endl;
                    SelectionParameters params = InputManager::createSelectionParameters(1, "Trash one card", g.HUDFormat(), SelectionMode::EXACTLY, {CardType::TREASURE}, localTreasures);
                    std::vector<const Card*> selection = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);
                    if(!selection.empty()){
                        selected = selection[0];
                    }
                    break;
            }
            if(selected){
                revealed.erase(std::find(revealed.begin(), revealed.end(), selected));
                treasures.push_back(selected);
            }

            for(const Card* c : revealed){
                player->addCardToDiscardStack(c);
            }
        }
    }

    if(treasures.empty()){
        std::cout << "You didn't find any treasure..." << std::endl;
    }
    else{
        std::cout << "Now you can keep those treasures!" << std::endl;
        SelectionParameters params = InputManager::createSelectionParameters(treasures.size(), "Steal as many treasures as you want", g.HUDFormat(), SelectionMode::AT_MOST, {CardType::TREASURE}, treasures);
        std::vector<const Card*> toGain = InputManager::chooseCardsFromSelection(p, g.getCurrentPhase(), params);
        for(const Card* c : treasures){
            if(std::find(toGain.begin(), toGain.end(), c) != toGain.end()){
                p.addCardToDiscardStack(c);
            }
            else{
                g.trashCard(c);
            }
        }
    }
}