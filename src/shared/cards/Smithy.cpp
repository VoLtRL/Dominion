#include "shared/cards/Smithy.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Smithy::play(Game&, Player& p) const {
    std::cout << "You draw 3 cards!" << std::endl;
    p.drawCardsToHand(3);
}