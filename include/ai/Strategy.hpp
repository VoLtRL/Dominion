#pragma once
#include <vector>
#include "Phase.hpp"
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

        Strategy(std::string n, int t, std::vector<Phase> p, Phase u, Strategy* f, std::function<double(std::map<const Card*, int>)> v, std::map<std::string, int> pa)
        : name(n), tier(t), phases(p), urgency(u), viability(v), fallback(f), params(pa) {}

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
            if(fallback){
                fallback->finalize();
            }
            isFinalized = true;
        }

        static Strategy bigMoney(std::map<const Card*, int> cardDistribution) {
            // Phase 1 : On achète du silver et du copper

            std::vector<Rule> buyRules = {
                Rule(cond::canBuyCard(Factory::get("Silver")), Factory::get("Silver"), 1, "buy(Silver)"),
                Rule(cond::canBuyCard(Factory::get("Copper")), Factory::get("Copper"), 0, "buy(Copper)"),
            };
            std::vector<Rule> playRules;
            std::vector<DecisionRule> decisionRules = {
                DecisionRule(DecisionType::DISCARD, cond::always(), {Factory::get("Curse"), Factory::get("Estate"), Factory::get("Copper"), Factory::get("Silver")}, "discard"),
                DecisionRule(DecisionType::GAIN, cond::always(), {Factory::get("Gold"), Factory::get("Silver"), Factory::get("Copper")}, "gain")
            };
            Phase start("start", cond::always(), buyRules, playRules, decisionRules);

            // Phase 2 : On achète du gold et du silver

            buyRules = {
                Rule(cond::canBuyCard(Factory::get("Gold")), Factory::get("Gold"), 2, "buy(Gold)"),
                Rule(cond::canBuyCard(Factory::get("Silver")), Factory::get("Silver"), 1, "buy(Silver)"),
                Rule(cond::always(), Factory::get("Copper"), 0, "buy(Copper)"),
            };

            Phase build("build", cond::turnAtLeast(3), buyRules, playRules, decisionRules);

            // Phase 3 : On achète des provinces et du gold

            buyRules = {
                Rule(cond::canBuyCard(Factory::get("Province")), Factory::get("Province"), 4, "buy(Province)"),
                Rule(cond::canBuyCard(Factory::get("Duchy")) && cond::provincesLeftAtMost(3), Factory::get("Duchy"), 3, "buy(Duchy)"),
                Rule(cond::canBuyCard(Factory::get("Gold")), Factory::get("Gold"), 2, "buy(Gold)"),
                Rule(cond::canBuyCard(Factory::get("Silver")), Factory::get("Silver"), 1, "buy(Silver)"),
                Rule(cond::always(), Factory::get("Copper"), 0, "buy(Copper)"),
            };

            Phase green("green", cond::moneyDensityAtLeast(2.0), buyRules, playRules, decisionRules);

            // Phase 4 : On achète des provinces et des duchés

            buyRules = {
                Rule(cond::canBuyCard(Factory::get("Province")), Factory::get("Province"), 4, "buy(Province)"),
                Rule(cond::canBuyCard(Factory::get("Duchy")), Factory::get("Duchy"), 3, "buy(Duchy)"),
                Rule(cond::canBuyCard(Factory::get("Estate")), Factory::get("Estate"), 2, "buy(Estate)"),
                Rule(cond::canBuyCard(Factory::get("Gold")), Factory::get("Gold"), 1, "buy(Gold)"),
                Rule(cond::always(), Factory::get("Copper"), 0, "buy(Copper)"),
            };

            decisionRules = {
                DecisionRule(DecisionType::DISCARD, cond::always(), {Factory::get("Curse"),Factory::get("Copper"), Factory::get("Silver"), Factory::get("Estate")}, "discard"),
                DecisionRule(DecisionType::GAIN, cond::always(), {Factory::get("Province"), Factory::get("Duchy"), Factory::get("Estate"), Factory::get("Gold"), Factory::get("Silver"), Factory::get("Copper")}, "gain")
            };

            Phase endgame("endgame", cond::provincesLeftAtMost(3), buyRules, playRules, decisionRules);

            // Urgency : des règles prioritaires

            std::vector<DecisionRule> urgencyDecisionRules = DecisionRule::basicUrgencyDecisionRules();
            std::vector<Rule> urgencyBuyRules = Rule::basicUrgencyBuyRules(cardDistribution);
            Phase urgency("urgency", cond::always(), urgencyBuyRules, {}, urgencyDecisionRules);

            std::vector<Phase> phases = {start, build, green, endgame};
            Strategy big("bigMoney", 1, phases, urgency, nullptr, [](std::map<const Card*, int> distrib){return 1;}, {});
            return big;
        }
};