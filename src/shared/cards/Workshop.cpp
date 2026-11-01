#include "shared/cards/Workshop.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Workshop::play(Game& g, Player& p) const {
    g.requestSelection(&p, g.getBuyableCardsBelowCost(4), 1,
                       SelectionMode::GAIN, SelectionConstraint::EXACTLY,
                       Game::SelectionContext::WORKSHOP_GAIN);
}