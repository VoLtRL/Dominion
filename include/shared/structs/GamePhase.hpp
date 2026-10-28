#pragma once

#include <string>

enum class GamePhase{
    ACTION,
    BUY,
    REACTION,
    SELECTION,
    ATTACK
};

inline std::string toString(GamePhase phase) {
    switch (phase) {
        case GamePhase::ACTION: return "ACTION";
        case GamePhase::BUY: return "BUY";
        case GamePhase::REACTION: return "REACTION";
        case GamePhase::SELECTION: return "SELECTION";
        case GamePhase::ATTACK: return "ATTACK";
    }
    return "UNKNOWN";
}
