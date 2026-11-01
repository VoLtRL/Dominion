#include "shared/cards/Remodel.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Remodel::play(Game& g, Player& p) const {
    g.requestSelection(&p, p.getHand().getCards(), 1,
                       SelectionMode::TRASH, SelectionConstraint::EXACTLY,
                       Game::SelectionContext::REMODEL_TRASH);
}