#pragma once

#include "Condition.hpp"

class Phase {
    private:
        std::string name;
        Condition enter;
        std::vector<Rule> buyRules;
        std::vector<Rule> playRules;
        std::vector<DecisionRule> decisionRules;
    public:
        void finalize() {
            auto byPriority = [](const Rule& a, const Rule& b) { return a.getPriority() > b.getPriority(); };
            std::stable_sort(buyRules.begin(), buyRules.end(), byPriority);
            std::stable_sort(playRules.begin(), playRules.end(), byPriority);
        }

        bool canEnter(const GameView& view, int i) const {
            return enter(view, i);
        }

        std::vector<Rule> getBuyRules() const {
            return buyRules;
        }

        std::vector<Rule> getPlayRules() const {
            return playRules;
        }

        std::vector<DecisionRule> getDecisionRules() const {
            return decisionRules;
        }

        std::string getName() const{
            return name;
        }
    };