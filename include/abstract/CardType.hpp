enum class CardType {
    ACTION,
    TREASURE,
    VICTORY,
    CURSE,
    ATTACK,
    REACTION
};

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