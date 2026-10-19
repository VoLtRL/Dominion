#pragma once

#include "../player/Player.hpp"
#include "GameEvent.hpp"

class Treasure{
    public:
        virtual int getCoins() const = 0;
};