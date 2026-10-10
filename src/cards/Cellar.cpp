#include "../../include/cards/Cellar.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Cellar::onPlay(Game &g, Player &p) const {
  (void)g;
  while (p.getHand().getSize() > 0) {
    p.discardCard(p.getHand().getCards().front());
  }
}
