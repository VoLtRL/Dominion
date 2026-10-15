#include "../../include/cards/Thief.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

#include <algorithm>

void Thief::onPlay(Game& g, Player& p) const {
    std::cout << "Each other player reveals the top 2 cards of their deck. If there are Treasure Cards, you choose one of them to trash. You may gain any or all trashed Treasures, and the non-treasure cards revealed are discarded." << std::endl;
    std::vector<const Card*> treasures;
    for(Player player : g.getPlayers()){
        if(player!=p){
            std::vector<const Card*> revealed = {player.drawCard(), player.drawCard()};
            std::cout << "Revealed cards : " << std::endl;
            g.displaySelection(revealed);

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
                    selected = g.chooseCardsFromSelection(localTreasures, 1, "trash", "exact")[0];
                    break;
            }
            if(selected){
                revealed.erase(std::find(revealed.begin(), revealed.end(), selected));
                treasures.push_back(selected);
            }

            for(const Card* c : revealed){
                player.addCardToDiscardStack(c);
            }
        }
    }

    if(treasures.empty()){
        std::cout << "You didn't find any treasure..." << std::endl;
    }
    else{
        std::cout << "Now you can keep those treasures!" << std::endl;
        std::vector<const Card*> toGain = g.chooseCardsFromSelection(treasures, treasures.size(), "gain", "most");
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