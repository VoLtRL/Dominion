#include "../../include/cards/Moat.hpp"
#include "../../include/gameplay/Game.hpp"
#include "../../include/player/Player.hpp"

void Moat::onPlay(Game &g, Player &p) const {
  // Draw 2 cards
  p.drawCardsToHand(2);
}

bool Moat::canReact(GameEvent *event, Game *game, Player *owner) const {
  return event->getType() == EventType::AttackPlayed;
}

void Moat::react(GameEvent *event, Game *game, Player *owner) const {
  event->ignorePlayer(*owner);
}
