#pragma once

#include "Condition.hpp"

class Rule {
    private:
        Condition valid;
        const Card* card;
        int priority;
        std::string label;
    
    public:

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

};

enum class DecisionType{
    DISCARD,
    SIDE,
    GAIN,
    REACTION
};

class DecisionRule {
    private:
        Condition valid;
        std::vector<const Card*> ranking;
        std::string label;
        DecisionType type = DecisionType::DISCARD;
    public:
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


};