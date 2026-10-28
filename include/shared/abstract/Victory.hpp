#pragma once

#include "shared/player/Player.hpp"

class Victory{
    public:
        virtual int getVictoryPoints(const Player& p) const = 0;
};