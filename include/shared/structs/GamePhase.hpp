#pragma once

#include <string>

enum class GamePhase{
    ACTION,
    BUY,
    UNKNOWN
};

enum class PendingInteractionPhase{
    NONE,
    REACTION,
    SELECTION,
    ATTACK_RESOLVE
};

inline std::string toString(GamePhase phase) {
    switch (phase) {
        case GamePhase::ACTION: return "ACTION";
        case GamePhase::BUY: return "BUY";
        case GamePhase::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

static inline GamePhase fromString(const std::string& phaseStr) {
    if (phaseStr == "ACTION") return GamePhase::ACTION;
    if (phaseStr == "BUY") return GamePhase::BUY;
    if (phaseStr == "UNKNOWN") return GamePhase::UNKNOWN;
    throw std::invalid_argument("Invalid game phase string: " + phaseStr);
}

inline std::string toString(PendingInteractionPhase phase) {
    switch (phase) {
        case PendingInteractionPhase::NONE: return "NONE";
        case PendingInteractionPhase::REACTION: return "REACTION";
        case PendingInteractionPhase::SELECTION: return "SELECTION";
        case PendingInteractionPhase::ATTACK_RESOLVE: return "ATTACK_RESOLVE";
        default: return "NONE";
    }
}

static inline PendingInteractionPhase fromStringPendingInteractionPhase(const std::string& phaseStr) {
    if (phaseStr == "NONE") return PendingInteractionPhase::NONE;
    if (phaseStr == "REACTION") return PendingInteractionPhase::REACTION;
    if (phaseStr == "SELECTION") return PendingInteractionPhase::SELECTION;
    if (phaseStr == "ATTACK_RESOLVE") return PendingInteractionPhase::ATTACK_RESOLVE;
    throw std::invalid_argument("Invalid pending interaction phase string: " + phaseStr);
}
