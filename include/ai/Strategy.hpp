#pragma once
#include <vector>
#include "Phase.hpp"
#include "Rule.hpp"
#include <cmath>
#include <functional>

class Strategy{
    private:
        std::string name;
        int tier = 1;
        std::vector<Phase> phases;
        size_t phaseIndex = 0;
        Phase urgency;
        std::function<double(std::map<const Card*, int>)> viability;
        Strategy* fallback;
        bool isFinalized = false;
        std::map<std::string, int> params;
    public:

        size_t resolvePhase(const GameView& view, int i);
        size_t resolvePhase(const GameView& view, int i, size_t current);

        std::string getName() const { return name; }
        int getTier() const { return tier; }

        void setName(std::string n) { name = n; }
        void setTier(int t) { tier = t; }
        void setViability(std::function<double(std::map<const Card*, int>)> v) { viability = v; }
        void setParams(std::map<std::string, int> p) { params = p; }

        double getViability(std::map<const Card*, int> distrib) const;

        Phase getCurrentPhase() const{
            return phases[phaseIndex];
        }

        std::vector<Rule> getBuyRules() const{
            return getCurrentPhase().getBuyRules();
        }
        std::vector<Rule> getPlayRules() const{
            return getCurrentPhase().getPlayRules();
        }
        std::vector<DecisionRule> getDecisionRules() const{
            return getCurrentPhase().getDecisionRules();
        }

        Phase getUrgency() const {
            return urgency;
        }

        Strategy* getFallback() const {
            return fallback;
        }

        void finalize() {
            if(isFinalized){return;}
            for(Phase p : phases){
                p.finalize();
            }
            urgency.finalize();
            fallback->finalize();
            isFinalized = true;
        }
};