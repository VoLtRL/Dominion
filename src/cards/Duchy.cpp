#include "../../include/cards/Duchy.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Duchy::onGain(Game& g, Player& p) const {
    p.addVictoryPoints(3);
}

void Duchy::onTrash(Game& g, Player& p) const{
    p.addVictoryPoints(-3);
}