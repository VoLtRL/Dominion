#pragma once

#include "../player/Player.hpp"
#include "GameEvent.hpp"

class Cursed{
    public:
        virtual int getCursePoints(const Player& p) const = 0;
};