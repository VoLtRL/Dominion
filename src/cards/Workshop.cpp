#include "../../include/cards/Workshop.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Workshop::onPlay(Game& g, Player& p) const {
    (void)g;
    p.addCardToDiscardStack(Factory::get("Copper"));
}