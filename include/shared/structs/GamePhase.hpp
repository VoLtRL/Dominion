#pragma once

#include <string>

enum class GamePhase{
    ACTION,
    BUY,
    REACTION,
    SELECTION,
    ATTACK,
    UNKNOWN
};

inline std::string toString(GamePhase phase) {
    switch (phase) {
        case GamePhase::ACTION: return "ACTION";
        case GamePhase::BUY: return "BUY";
        case GamePhase::REACTION: return "REACTION";
        case GamePhase::SELECTION: return "SELECTION";
        case GamePhase::ATTACK: return "ATTACK";
        case GamePhase::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

static inline GamePhase fromString(const std::string& phaseStr) {
    if (phaseStr == "ACTION") return GamePhase::ACTION;
    if (phaseStr == "BUY") return GamePhase::BUY;
    if (phaseStr == "REACTION") return GamePhase::REACTION;
    if (phaseStr == "SELECTION") return GamePhase::SELECTION;
    if (phaseStr == "ATTACK") return GamePhase::ATTACK;
    if (phaseStr == "UNKNOWN") return GamePhase::UNKNOWN;
    throw std::invalid_argument("Invalid game phase string: " + phaseStr);
}
