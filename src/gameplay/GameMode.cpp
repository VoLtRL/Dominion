#include "../../include/gameplay/GameMode.hpp"

GameMode::GameMode(const std::string& modeName,
                   const std::string& modeDescription,
                   const std::map<const Card*, int>& distribution)
    : name(modeName), description(modeDescription), cardDistribution(distribution) {}

const std::string& GameMode::getName() const {
    return name;
}

const std::string& GameMode::getDescription() const {
    return description;
}

const std::map<const Card*, int>& GameMode::getCardDistribution() const {
    return cardDistribution;
}
