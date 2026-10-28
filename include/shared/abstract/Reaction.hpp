#pragma once

#include "shared/player/Player.hpp"
#include "shared/structs/EventType.hpp"


class Reaction {
  private:
    std::set<EventType> reactionTypes;
  public:
    bool canReact(EventType type, Player *owner) const {
      return isEventTriggered(reactionTypes, type);
    }
    virtual void react(EventType type, Game &game, Player *owner, Player* actor) const = 0;
    std::set<EventType> getTypes() const {
      return reactionTypes;
    }
    void setReactionTypes(std::set<EventType> t) {
      reactionTypes = t;
    }
};
