#pragma once

#include <map>
#include <set>
#include <string>

// Liste unique des types : enum, toString et fromString en sont générés.
#define EVENT_TYPES(X)          \
    X(ATTACK_PLAYED_SELF)          \
    X(ATTACK_PLAYED_OTHER)         \
    X(ATTACK_PLAYED_ANY)           \
                                   \
    X(TURN_START_SELF)             \
    X(TURN_START_OTHER)            \
    X(TURN_START_ANY)              \
                                   \
    X(CARD_GAINED_SELF)            \
    X(CARD_GAINED_OTHER)           \
    X(CARD_GAINED_ANY)             \
                                   \
    X(CARD_GAINED_2TYPES_SELF)     \
    X(CARD_GAINED_2TYPES_OTHER)    \
    X(CARD_GAINED_2TYPES_ANY)      \
                                   \
    X(CARD_TRASHED_SELF)           \
    X(CARD_TRASHED_OTHER)          \
    X(CARD_TRASHED_ANY)            \
                                   \
    X(TREASURE_GAINED_SELF)        \
    X(TREASURE_GAINED_OTHER)       \
    X(TREASURE_GAINED_ANY)         \
                                   \
    X(VICTORY_GAINED_SELF)         \
    X(VICTORY_GAINED_OTHER)        \
    X(VICTORY_GAINED_ANY)          \
                                   \
    X(PROVINCE_GAINED_SELF)        \
    X(PROVINCE_GAINED_OTHER)       \
    X(PROVINCE_GAINED_ANY)

enum class EventType {
#define X(name) name,
    EVENT_TYPES(X)
#undef X
    INVALID_EVENT
};

using EventTypeSet = std::set<EventType>;

inline std::string eventTypeToString(EventType type) {
    switch (type) {
#define X(name) case EventType::name: return #name;
        EVENT_TYPES(X)
#undef X
        default: return "INVALID_EVENT";
    }
}

inline EventType stringToEventType(const std::string &str) {
#define X(name) if (str == #name) return EventType::name;
    EVENT_TYPES(X)
#undef X
    return EventType::INVALID_EVENT;
}

// Renvoie tous les types de réaction validés par un événement de type t
// (t lui-même, sa variante ANY, et les types plus généraux).
inline const EventTypeSet& getAllEventTypesFromType(EventType t) {
    using E = EventType;

    static const std::map<E, EventTypeSet> table = {
        // --- Événements simples : X_SELF / X_OTHER -> X_ANY ---
        {E::ATTACK_PLAYED_SELF,  {E::ATTACK_PLAYED_SELF,  E::ATTACK_PLAYED_ANY}},
        {E::ATTACK_PLAYED_OTHER, {E::ATTACK_PLAYED_OTHER, E::ATTACK_PLAYED_ANY}},
        {E::ATTACK_PLAYED_ANY,   {E::ATTACK_PLAYED_ANY}},

        {E::TURN_START_SELF,     {E::TURN_START_SELF,     E::TURN_START_ANY}},
        {E::TURN_START_OTHER,    {E::TURN_START_OTHER,    E::TURN_START_ANY}},
        {E::TURN_START_ANY,      {E::TURN_START_ANY}},

        {E::CARD_TRASHED_SELF,   {E::CARD_TRASHED_SELF,   E::CARD_TRASHED_ANY}},
        {E::CARD_TRASHED_OTHER,  {E::CARD_TRASHED_OTHER,  E::CARD_TRASHED_ANY}},
        {E::CARD_TRASHED_ANY,    {E::CARD_TRASHED_ANY}},

        // CARD_GAINED_2TYPES : dépend de la carte, émis à part (pas de parent)
        {E::CARD_GAINED_2TYPES_SELF,  {E::CARD_GAINED_2TYPES_SELF,  E::CARD_GAINED_2TYPES_ANY}},
        {E::CARD_GAINED_2TYPES_OTHER, {E::CARD_GAINED_2TYPES_OTHER, E::CARD_GAINED_2TYPES_ANY}},
        {E::CARD_GAINED_2TYPES_ANY,   {E::CARD_GAINED_2TYPES_ANY}},

        // --- Chaîne : PROVINCE -> VICTORY -> CARD, TREASURE -> CARD ---
        {E::CARD_GAINED_SELF,   {E::CARD_GAINED_SELF,  E::CARD_GAINED_ANY}},
        {E::CARD_GAINED_OTHER,  {E::CARD_GAINED_OTHER, E::CARD_GAINED_ANY}},
        {E::CARD_GAINED_ANY,    {E::CARD_GAINED_ANY}},

        {E::TREASURE_GAINED_SELF,  {E::TREASURE_GAINED_SELF,  E::TREASURE_GAINED_ANY,
                                    E::CARD_GAINED_SELF,      E::CARD_GAINED_ANY}},
        {E::TREASURE_GAINED_OTHER, {E::TREASURE_GAINED_OTHER, E::TREASURE_GAINED_ANY,
                                    E::CARD_GAINED_OTHER,     E::CARD_GAINED_ANY}},
        {E::TREASURE_GAINED_ANY,   {E::TREASURE_GAINED_ANY,   E::CARD_GAINED_ANY}},

        {E::VICTORY_GAINED_SELF,   {E::VICTORY_GAINED_SELF,   E::VICTORY_GAINED_ANY,
                                    E::CARD_GAINED_SELF,      E::CARD_GAINED_ANY}},
        {E::VICTORY_GAINED_OTHER,  {E::VICTORY_GAINED_OTHER,  E::VICTORY_GAINED_ANY,
                                    E::CARD_GAINED_OTHER,     E::CARD_GAINED_ANY}},
        {E::VICTORY_GAINED_ANY,    {E::VICTORY_GAINED_ANY,    E::CARD_GAINED_ANY}},

        {E::PROVINCE_GAINED_SELF,  {E::PROVINCE_GAINED_SELF,  E::PROVINCE_GAINED_ANY,
                                    E::VICTORY_GAINED_SELF,   E::VICTORY_GAINED_ANY,
                                    E::CARD_GAINED_SELF,      E::CARD_GAINED_ANY}},
        {E::PROVINCE_GAINED_OTHER, {E::PROVINCE_GAINED_OTHER, E::PROVINCE_GAINED_ANY,
                                    E::VICTORY_GAINED_OTHER,  E::VICTORY_GAINED_ANY,
                                    E::CARD_GAINED_OTHER,     E::CARD_GAINED_ANY}},
        {E::PROVINCE_GAINED_ANY,   {E::PROVINCE_GAINED_ANY,
                                    E::VICTORY_GAINED_ANY,
                                    E::CARD_GAINED_ANY}},
    };

    static const EventTypeSet empty;
    auto it = table.find(t);
    return it != table.end() ? it->second : empty;
}

// Renvoie l'union de tous les types validés par chacun des types donnés.
inline EventTypeSet getAllEventTypesFromTypes(const EventTypeSet& types) {
    EventTypeSet result;

    for (EventType t : types) {
        const EventTypeSet& validated = getAllEventTypesFromType(t);
        result.insert(validated.begin(), validated.end());
    }

    return result;
}

// Checks if the incoming event triggers the typeSet
inline bool isEventTriggered(const EventTypeSet& typeSet, const EventType incoming) {
    EventTypeSet allIncoming = getAllEventTypesFromType(incoming);
    for(EventType t : typeSet) {
        if(allIncoming.count(t) > 0) {
            return true;
        }
    }
    return false;
}