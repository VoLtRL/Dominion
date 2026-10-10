#include "../../include/gameplay/Game.hpp"
#include "../../include/cards/Chancellor.hpp"
#include "../../include/player/Player.hpp"

void Chancellor::onPlay(Game &g, Player &p) const
{
    g.addGold(2);
    while (p.getDrawStack().getSize() > 0) {
        p.drawCard();
        p.discardCard(p.getHand().getCards().back());
    }
}
