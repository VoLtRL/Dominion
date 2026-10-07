#pragma once

enum class CardType {
    Action,
    Treasure,
    Victory,
    Curse,
    Attack,
    Reaction
};

inline constexpr std::string_view cardTypeNames[] = {
    "Action","Treasure","Victory","Curse","Attack","Reaction"
};

inline std::string_view toString(CardType t) {
    return cardTypeNames[static_cast<std::size_t>(t)];
}