#pragma once
#include <string>

/**
 * @brief Represents the type of a card in the game.
 */
enum class CardType {
    ACTION,
    TREASURE,
    VICTORY,
    CURSE,
    ATTACK,
    REACTION
};

/**
 * @brief Converts a CardType to its string representation.
 * @param t The CardType to convert.
 * @return A string representing the CardType.
 */
inline std::string tostring(CardType t) {
    switch (t) {
        case CardType::ACTION: return "Action";
        case CardType::TREASURE: return "Treasure";
        case CardType::VICTORY: return "Victory";
        case CardType::CURSE: return "Curse";
        case CardType::ATTACK: return "Attack";
        case CardType::REACTION: return "Reaction";
        default: return "Unknown";
    }
}