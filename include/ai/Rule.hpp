#pragma once

#include "Condition.hpp"

class Rule {
    private:
        Condition valid;
        int priority;
        std::string label;
    
    public:

        bool applies(const GameView& v, int i) const {
            return valid(v,i);
        }

};