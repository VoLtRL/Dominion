#pragma once

#include "Condition.hpp"

class Rule {
    private:
        Condition valid;
        const Card* card;
        int priority;
        std::string label;
    
    public:

        Rule(Condition v, const Card* c, int p, std::string l) {
            valid = v;
            card = c;
            priority = p;
            label = l;
        }

        bool applies(const GameView& v, int i) const {
            return valid(v,i);
        }

        bool isCardInSelection(std::vector<const Card*> selection){
            return std::find(selection.begin(), selection.end(), card) != selection.end();
        }

        int getPriority() const{
            return priority;
        }

        const Card* getCard() const {
            return card;
        }

        std::string getLabel() const {
            return label;
        }

        
        static std::vector<Rule> basicUrgencyBuyRules(std::map<const Card*, int> cardDistribution){
            std::vector<Rule> urgencyRules;
            urgencyRules.push_back(Rule(cond::aheadBy(1) && cond::canEmptyPile(Factory::get("Province")), Factory::get("Province"), 100, "finish_game_province"));
            for(auto data : cardDistribution){
                const Card* card = data.first;
                urgencyRules.push_back(Rule(cond::emptyPilesAtLeast(2) && cond::aheadBy(1) && cond::canEmptyPile(card), card, 100, "finish_game(" + card->getName() + ")"));
                urgencyRules.push_back(Rule(cond::emptyPilesAtLeast(1) && cond::aheadBy(3) && cond::canEmptyPile(card), card, 90, "empty(" + card->getName() + ")"));
                urgencyRules.push_back(Rule(cond::aheadBy(5) && cond::canEmptyPile(card), card, 80, "ahead(" + card->getName() + ")"));
                urgencyRules.push_back(Rule(cond::aheadBy(5) && cond::leftAtMost(card, 2), card, 70, "ahead(" + card->getName() + ")"));
            }
            return urgencyRules;
        }

};

enum class DecisionType{
    DISCARD,
    SIDE,
    GAIN,
    REACTION,
    TRASH,
    PLAY
};

class DecisionRule {
    private:
        Condition valid;
        std::vector<const Card*> ranking;
        std::string label;
        DecisionType type = DecisionType::DISCARD;
    public:

        DecisionRule(DecisionType t, Condition v, std::vector<const Card*> r, std::string l) {
            valid = v;
            ranking = r;
            label = l;
            type = t;
        }

        bool applies(const GameView& v, int i) const {
            return valid(v,i);
        }

        std::vector<const Card*> getCards() const {
            return ranking;
        }

        std::string getLabel() const {
            return label;
        }

        DecisionType getType() const {
            return type;
        }

        static std::vector<DecisionRule> basicUrgencyDecisionRules(){
            std::vector<DecisionRule> urgencyRules;
            urgencyRules.push_back(DecisionRule(DecisionType::DISCARD, cond::always(), {Factory::get("Curse")}, "discard_curse"));
            return urgencyRules;
        }
};