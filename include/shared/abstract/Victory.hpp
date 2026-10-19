#pragma once

#include "shared/player/Player.hpp"
#include "shared/abstract/GameEvent.hpp"

class Victory{
    public:
        virtual int getVictoryPoints(const Player& p) const = 0;
};