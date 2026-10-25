#pragma once

#include "Condition.hpp"

class Phase {
    private:
        std::string name;
        Condition enter;
        std::vector<BuyRule> buyRules;
        std::vector<PlayRule> playRules;
        std::vector<DecisionRule> decisionRules;
    public:

};