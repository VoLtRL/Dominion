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
    REACTION,
    ALL,
    NONE
};

/**
 * @brief Converts a CardType to its string representation.
 * @param t The CardType to convert.
 * @return A string representing the CardType.
 */
inline std::string cardTypeToString(CardType t) {
    switch (t) {
        case CardType::ACTION: return "Action";
        case CardType::TREASURE: return "Treasure";
        case CardType::VICTORY: return "Victory";
        case CardType::CURSE: return "Curse";
        case CardType::ATTACK: return "Attack";
        case CardType::REACTION: return "Reaction";
        case CardType::ALL: return "All";
        default: return "NONE";
    }
}

/**
 * @brief Converts a string to the corresponding CardType.
 * @param s The string to convert.
 * @return The corresponding CardType, NONE by default.
 */
inline CardType stringToCardType(const std::string &s) {
    if(s == "Action"){ return CardType::ACTION; }
    if(s == "Treasure"){ return CardType::TREASURE; }
    if(s == "Victory"){ return CardType::VICTORY; }
    if(s == "Curse"){ return CardType::CURSE; }
    if(s == "Attack"){ return CardType::ATTACK; }
    if(s == "Reaction"){ return CardType::REACTION; }
    if(s == "All"){ return CardType::ALL; }
    return CardType::NONE;
}