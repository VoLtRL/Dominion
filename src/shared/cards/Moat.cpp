#include "shared/cards/Moat.hpp"
#include "shared/gameplay/Game.hpp"
#include "shared/player/Player.hpp"

void Moat::play(Game& g, Player& p) const {
    // Draw 2 cards
    std::cout << "You draw 2 cards !" << std::endl;
    p.drawCardsToHand(2);
}

bool Moat::canReact(GameEvent *event, Game *game, Player *owner) const {
  return event->getType() == EventType::AttackPlayed;
}

void Moat::react(GameEvent* event, Game* game, Player* owner) const {
  if (event->getSource() == *owner){
    std::cout << "Player " << owner->getName() << " is the source of the attack, so he won't be ignored !" << std::endl;
    return;
  }
  std::cout << "Player " << owner->getName() << " is immuned to this attack !" << std::endl;
  event->ignorePlayer(*owner);
}
