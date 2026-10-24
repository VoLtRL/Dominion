#pragma once

#include <string>

enum class ActionType {
    CREATE_ROOM,
    ROOM_CREATED,
    ROOM_CREATION_FAILED,
    ROOM_JOIN_FAILED,
    REQUEST_ROOM_LIST,
    JOIN_ROOM,
    PLAYER_JOINED,
    LEAVE_ROOM,
    PLAYER_LEFT,
    START_GAME,
    PLAY_CARD,
    END_TURN,
    INVALID_ACTION,
};

inline std::string actionTypeToString(ActionType actionType) {
    switch (actionType) {
        case ActionType::CREATE_ROOM: return "CREATE_ROOM";
        case ActionType::ROOM_CREATED: return "ROOM_CREATED";
        case ActionType::ROOM_CREATION_FAILED: return "ROOM_CREATION_FAILED";
        case ActionType::ROOM_JOIN_FAILED: return "ROOM_JOIN_FAILED";
        case ActionType::REQUEST_ROOM_LIST: return "REQUEST_ROOM_LIST";
        case ActionType::JOIN_ROOM: return "JOIN_ROOM";
        case ActionType::PLAYER_JOINED: return "PLAYER_JOINED";
        case ActionType::LEAVE_ROOM: return "LEAVE_ROOM";
        case ActionType::PLAYER_LEFT: return "PLAYER_LEFT";
        case ActionType::START_GAME: return "START_GAME";
        case ActionType::PLAY_CARD: return "PLAY_CARD";
        case ActionType::END_TURN: return "END_TURN";
        default: return "INVALID_ACTION";
    }
}

inline ActionType stringToActionType(const std::string &actionStr) {
    if (actionStr == "CREATE_ROOM") return ActionType::CREATE_ROOM;
    if (actionStr == "ROOM_CREATED") return ActionType::ROOM_CREATED;
    if (actionStr == "ROOM_CREATION_FAILED") return ActionType::ROOM_CREATION_FAILED;
    if (actionStr == "ROOM_JOIN_FAILED") return ActionType::ROOM_JOIN_FAILED;
    if (actionStr == "REQUEST_ROOM_LIST") return ActionType::REQUEST_ROOM_LIST;
    if (actionStr == "JOIN_ROOM") return ActionType::JOIN_ROOM;
    if (actionStr == "PLAYER_JOINED") return ActionType::PLAYER_JOINED;
    if (actionStr == "LEAVE_ROOM") return ActionType::LEAVE_ROOM;
    if (actionStr == "PLAYER_LEFT") return ActionType::PLAYER_LEFT;
    if (actionStr == "START_GAME") return ActionType::START_GAME;
    if (actionStr == "PLAY_CARD") return ActionType::PLAY_CARD;
    if (actionStr == "END_TURN") return ActionType::END_TURN;
    return ActionType::INVALID_ACTION;
}