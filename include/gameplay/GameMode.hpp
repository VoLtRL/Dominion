#pragma once

#include <string>
#include <map>
#include <memory>

#include "../abstract/Card.hpp"

class GameMode {
    private:
        std::string name;
        std::string description;
        std::map<const Card*, int> cardDistribution;

    public:
        GameMode(const std::string& name, const std::string& description, const std::map<const Card*, int>& cardDistribution);
        const std::string& getName() const;
        const std::string& getDescription() const;
        const std::map<const Card, int>& getCardDistribution() const;
};