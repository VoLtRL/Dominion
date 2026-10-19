#pragma once

#include "shared/player/Player.hpp"
#include "shared/abstract/GameEvent.hpp"

class Attack{
    public:
        virtual void attack(Game &g, Player &p, std::set<Player *> targets) const = 0;
};