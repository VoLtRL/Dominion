#include "../../include/cards/Smithy.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Smithy::play(Game&, Player& p) const {
    std::cout << "You draw 3 cards!" << std::endl;
    p.drawCardsToHand(3);
}