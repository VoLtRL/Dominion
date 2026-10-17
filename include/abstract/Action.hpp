#pragma once

#include "../player/Player.hpp"
#include "GameEvent.hpp"


class Action{
    public:
        virtual void play(Game &g, Player &p) const = 0;
};