#pragma once
#include <vector>
#include <string>

#include "GamePhase.hpp"

#include "../player/Player.hpp"
#include "../abstract/Card.hpp"
class Bot;

enum class SelectionMode{
    EXACTLY,
    AT_MOST,
    AT_LEAST
};

struct SelectionParameters{
    size_t nbToSelect;
    std::string label;
    std::string presentation;
    SelectionMode mode;
    std::vector<const Card*> selection;
    bool allowAll;
};

class InputManager{
    private : 
        static const bool isConsole = true; //true = mode console

    public:

        static bool getInputType() {
            return isConsole;
        }

        static SelectionParameters createSelectionParameters(size_t nb, std::string label, std::string presentation, SelectionMode mode, std::set<CardType> allowedTypes, std::vector<const Card*> selection, bool allowAll=false){
            std::vector<const Card*> validSelection;
            if(allowedTypes.find(CardType::ALL)!=allowedTypes.end()){
                validSelection = selection;
            }
            else{
                for(auto c : selection){
                    for(auto t : c->getTypes()){
                        if(allowedTypes.find(t) != allowedTypes.end()){
                            validSelection.push_back(c);
                            break;
                        }
                    }
                }
            }
            return {nb, label, presentation, mode, validSelection, allowAll};
        }

        static std::vector<const Card*> chooseCardsFromSelection(Player& p, GamePhase phase, SelectionParameters parameters);

        static std::vector<std::string> parseLines(const std::string &str);
        static void displaySelection(const std::vector<const Card *> &selection);
};