#include "shared/cards/Chapel.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"
#include <algorithm>

void Chapel::play(Game& g, Player& p) const {

    std::cout << "You may trash up to 4 cards from your hand." << std::endl;
    g.requestSelection(&p, p.getHand().getCards(), std::min<size_t>(4, p.getHand().getSize()),
                       SelectionMode::TRASH, SelectionConstraint::AT_MOST,
                       Game::SelectionContext::CHAPEL_TRASH);

}