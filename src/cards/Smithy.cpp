#include "../../include/cards/Smithy.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Smithy::onPlay(Game&, Player& p) const {
    p.drawCards(3);
}