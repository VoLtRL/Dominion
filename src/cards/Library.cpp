#include "../../include/cards/Library.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Library::onPlay(Game&, Player& p) const {
    p.drawCards(3);
}