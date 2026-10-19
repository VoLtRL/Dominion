#pragma once

#include "shared/player/Player.hpp"
#include "shared/abstract/GameEvent.hpp"

class Cursed{
    public:
        virtual int getCursePoints(const Player& p) const = 0;
};