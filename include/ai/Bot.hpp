#pragma once

#include "../player/Player.hpp"
#include "Strategy.hpp"

class Bot : public Player {
    private:
        
    public:
        bool isHuman() const {return false;}
        void playTurn();

};