#pragma once

#include "shared/player/Player.hpp"


class Action{
    public:
        virtual void play(Game &g, Player &p) const = 0;
};