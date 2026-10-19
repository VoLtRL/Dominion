#pragma once

#include "shared/player/Player.hpp"
#include "shared/abstract/GameEvent.hpp"

class GameEvent;

class Reaction {
public:
  virtual bool canReact(GameEvent *event, Game *game, Player *owner) const = 0;
  virtual void react(GameEvent *event, Game *game, Player *owner) const = 0;
};
