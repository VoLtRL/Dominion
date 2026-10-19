#pragma once

#include "../player/Player.hpp"
#include "GameEvent.hpp"

class Attack{
    public:
        virtual void attack(Game &g, Player &p, std::set<Player *> targets) const = 0;
};