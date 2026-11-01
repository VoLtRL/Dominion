#include "shared/cards/Cellar.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Cellar::play(Game &g, Player &p) const {
  std::cout << "You get one additional action and you may discard any number of cards from your hand and draw one for every card discarded this way." << std::endl;
  g.addPlayerActions(1);

  g.requestSelection(&p, p.getHand().getCards(), p.getHand().getSize(),
                     SelectionMode::DISCARD, SelectionConstraint::AT_MOST,
                     Game::SelectionContext::CELLAR_DISCARD);
}
