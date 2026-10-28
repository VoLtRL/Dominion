#include "shared/cards/Moat.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Moat::play(Game& g, Player& p) const {
    // Draw 2 cards
    std::cout << "You draw 2 cards !" << std::endl;
    p.drawCardsToHand(2);
}

void Moat::react(EventType type, Game& game, Player* owner, Player* actor) const {
  if(owner == actor) { return; }
  game.applyProtection(owner, actor, this);
}
