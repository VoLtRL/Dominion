#include "../../include/cards/Spy.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Spy::onPlay(Game& g, Player& p) const {
    p.drawCards(1);
    g.addPlayerActions(1);
}