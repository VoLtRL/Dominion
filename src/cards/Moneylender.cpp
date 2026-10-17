#include "../../include/cards/Moneylender.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Moneylender::play(Game& g, Player& p) const {
    std::cout << "You may trash a Copper from your hand to gain 3 coins this turn." << std::endl;
    bool found = false;
    for(const Card* c : p.getHand().getCards()){
        if(c->getName() == "Copper"){
            std::string input;
            while(true){
                std::cout << "Copper found. Do you want to trash it ? (Y/N)" << std::endl;
                std::cin >> input;
                if(input == "Y" || input == "y"){
                    std::cout << "Card trashed! Enjoy your 3 coins." << std::endl;
                    p.removeCardFromHand(c);
                    g.trashCard(c);
                    g.addGold(3);
                    break;
                }
                if(input == "N" || input == "n"){
                    std::cout << "Card was not trashed." << std::endl;
                    break;
                }
                std::cout << "Please enter a valid response (Y/N)" << std::endl;
            }
            found = true;
            break;
        }
    }
    if(!found){
        std::cout << "You don't have any copper in your hand." << std::endl;
    }
}