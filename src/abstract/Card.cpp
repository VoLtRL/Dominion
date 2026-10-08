#include "../../include/abstract/Card.hpp"
#include "../../include/abstract/CardType.hpp"
#include <cmath>
#include <iostream>

void Card::isA(CardType t){
    std::set<Card> types = this->getTypes();
    return types.find(t) != types.end();
}

std::string wrapString(std::string text, size_t maxWidth){
    std::string lines;
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

std::string Card::getString(size_t maxWidth = 30) const {
    std::string typesStr;
    for (auto& t : getTypes){
        if(!typesStr.empty()){typesStr += "-";}
        typesStr += tostring(t);
    }
    std::vector<std::string> blocks = {
        wrapString(name,maxWidth),
        wrapString(description,maxWidth),
        wrapString("Cost : "+tostring(cost),maxWidth),
        wrapString(typesStr,maxWidth)
    }

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

void Card::display(){
    std::cout << getString() << std::endl;
}