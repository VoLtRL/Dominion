#pragma once

enum class SelectionMode {
    NONE,
    PLAY_CARD,
    DISCARD,
    TRASH,
    GAIN,
    REACTION
};

inline std::string toString(SelectionMode mode) {
    switch (mode) {
        case SelectionMode::NONE: return "NONE";
        case SelectionMode::PLAY_CARD: return "PLAY_CARD";
        case SelectionMode::DISCARD: return "DISCARD";
        case SelectionMode::TRASH: return "TRASH";
        case SelectionMode::GAIN: return "GAIN";
        case SelectionMode::REACTION: return "REACTION";
        default: return "NONE";
    }
}

static inline SelectionMode stringToSelectionMode(const std::string& modeStr) {
    if (modeStr == "NONE") return SelectionMode::NONE;
    if (modeStr == "PLAY_CARD") return SelectionMode::PLAY_CARD;
    if (modeStr == "DISCARD") return SelectionMode::DISCARD;
    if (modeStr == "TRASH") return SelectionMode::TRASH;
    if (modeStr == "GAIN") return SelectionMode::GAIN;
    if (modeStr == "REACTION") return SelectionMode::REACTION;
    throw std::invalid_argument("Invalid selection mode string: " + modeStr);
}

enum class SelectionConstraint {
    NONE,
    AT_LEAST,
    AT_MOST,
    EXACTLY
};

inline std::string toString(SelectionConstraint constraint) {
    switch (constraint) {
        case SelectionConstraint::NONE: return "NONE";
        case SelectionConstraint::AT_LEAST: return "AT_LEAST";
        case SelectionConstraint::AT_MOST: return "AT_MOST";
        case SelectionConstraint::EXACTLY: return "EXACTLY";
        default: return "NONE";
    }
}

static inline SelectionConstraint stringToSelectionConstraint(const std::string& constraintStr) {
    if (constraintStr == "NONE") return SelectionConstraint::NONE;
    if (constraintStr == "AT_LEAST") return SelectionConstraint::AT_LEAST;
    if (constraintStr == "AT_MOST") return SelectionConstraint::AT_MOST;
    if (constraintStr == "EXACTLY") return SelectionConstraint::EXACTLY;
    throw std::invalid_argument("Invalid selection constraint string: " + constraintStr);
}