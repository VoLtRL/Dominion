#include "shared/gameplay/Game.hpp"
#include "shared/cards/Chancellor.hpp"
#include "shared/player/Player.hpp"

void Chancellor::play(Game &g, Player &p) const
{
    std::cout << "You gain 2 coins for this turn and you may immediately discard your entire draw pile." << std::endl;
    g.addGold(2);
    std::string input;
    while (true) {
        std::cout << "Do you want to discard your entire draw pile ? (Y/N)" << std::endl;
        std::getline(std::cin, input);

        if (input == "Y" || input == "y") { 
            p.discardDrawStack();
            break; 
        }
        if (input == "N" || input == "n") {
            break; 
        }

        std::cout << "Please answer Y or N." << std::endl;
    }
}
