#pragma once

#include "../player/Player.hpp"
#include "GameEvent.hpp"

class Victory{
    public:
        virtual int getVictoryPoints(const Player& p) const = 0;
};