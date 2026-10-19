#pragma once

#include "shared/player/Player.hpp"
#include "shared/abstract/GameEvent.hpp"

class Treasure{
    public:
        virtual int getCoins() const = 0;
};