#include "../../include/abstract/Card.hpp"
#include "../../include/abstract/CardType.hpp"
#include <cmath>
#include <iostream>
#include <sstream>

void Card::onPlay(Game& g, Player& p) const {
    (void)g;
    (void)p;
}

void Card::onDiscard(Game& g, Player& p) const {
    (void)g;
    (void)p;
}

void Card::onTrash(Game& g, Player& p) const {
    (void)g;
    (void)p;
}

void Card::onGain(Game& g, Player& p) const {
    (void)g;
    (void)p;
}

bool Card::isA(CardType t) const{
    std::set<CardType> types = getTypes();
    return types.find(t) != types.end();
}

const std::vector<std::string> wrapString(std::string text, size_t maxWidth){
    std::vector<std::string> lines;
    std::istringstream words(text);
    std::string line;
    std::string word;

    while(words >> word){
        if(word.size() > maxWidth){
            if(!line.empty()){lines.push_back(line);line.clear();}
            lines.push_back(word.substr(0,maxWidth));
            word.erase(0,maxWidth);
        }
        if(line.empty()){
            line = word;
        }
        else if(line.size() + 1 + word.size() <= maxWidth){
            line += " " + word;
        }
        else{
            lines.push_back(line);
            line = word;
        }
    }
    if(!line.empty()){lines.push_back(line);}
    if(lines.empty()){lines.push_back("");}
    return lines;
}

const std::string Card::getString(size_t maxWidth) const {
    std::string typesStr;
    for (CardType t : getTypes()){
        if(!typesStr.empty()){typesStr += "-";}
        switch(t){
            case CardType::ACTION: typesStr += "Action"; break;
            case CardType::TREASURE: typesStr += "Treasure"; break;
            case CardType::VICTORY: typesStr += "Victory"; break;
            case CardType::CURSE: typesStr += "Curse"; break;
            case CardType::ATTACK: typesStr += "Attack"; break;
            case CardType::REACTION: typesStr += "Reaction"; break;
        }
    }
    std::vector<std::vector<std::string>> blocks = {
        wrapString(name,maxWidth),
        wrapString(description,maxWidth),
        wrapString("Cost : "+std::to_string(cost),maxWidth),
        wrapString(typesStr,maxWidth)
    };

    size_t width = 0;
    for(auto& block:blocks){
        for(auto& line:block){
            width = std::max(width,line.size());
        }
    }
    auto border = [&](char c) {
        return "+" + std::string(width + 2, c) + "+\n";
    };
    auto row = [&](const std::string& s) {
        const std::size_t total = width - s.size();
        const std::size_t left  = total / 2;
        const std::size_t right = total - left;
        return "| " + std::string(left, ' ') + s + std::string(right, ' ') + " |\n";
    };

    std::string out = border('_');
    for (const auto& block : blocks) {
        for (const auto& line : block){ 
            out += row(line);
        }
        out += border('-');
    }
    return out;

}

void Card::display() const{
    std::cout << getString(30) << std::endl;
}