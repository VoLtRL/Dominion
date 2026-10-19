#pragma once
#include "shared/abstract/GameEvent.hpp"

class AttackEvent : public GameEvent {
private:
  const Card *cardPlayed;
  const std::set<Player *> targets;

public:
  AttackEvent(const Card *c, Player &source, const std::set<Player *> ts)
      : GameEvent(EventType::AttackPlayed, source), cardPlayed(c),
        targets(ts) {};

  const std::set<Player *> getTargets() const { return targets; }
};
