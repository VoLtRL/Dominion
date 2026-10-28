#pragma once

#include <string>

enum class ActionType {

    CREATE_ROOM,
    ROOM_CREATED,
    ROOM_CREATION_FAILED,

    REQUEST_ROOM_LIST,

    JOIN_ROOM,
    PLAYER_JOINED,
    ROOM_JOIN_FAILED,

    LEAVE_ROOM,
    PLAYER_LEFT,

    START_GAME,

    // update game state for the client
    GAME_STATE,
    TURN_STATE,

    // client actions
    SELECTION_REQUEST,
    BUY_CARD,
    PLAY_CARD,
    END_TURN,
    REACTION,
    INVALID_ACTION,

    NEXT_PHASE,
    ERROR,
    GAME_ENDED,


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
        case ActionType::TURN_STATE: return "TURN_STATE";
        case ActionType::GAME_STATE: return "GAME_STATE";
        case ActionType::END_TURN: return "END_TURN";
        case ActionType::NEXT_PHASE: return "NEXT_PHASE";
        case ActionType::GAME_ENDED: return "GAME_ENDED";
        case ActionType::BUY_CARD: return "BUY_CARD";
        case ActionType::SELECTION_REQUEST: return "SELECTION_REQUEST";
        case ActionType::ERROR: return "ERROR";
        case ActionType::REACTION: return "REACTION";
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
    if (actionStr == "TURN_STATE") return ActionType::TURN_STATE;
    if (actionStr == "GAME_STATE") return ActionType::GAME_STATE;
    if (actionStr == "END_TURN") return ActionType::END_TURN;
    if (actionStr == "NEXT_PHASE") return ActionType::NEXT_PHASE;
    if (actionStr == "GAME_ENDED") return ActionType::GAME_ENDED;
    if (actionStr == "BUY_CARD") return ActionType::BUY_CARD;
    if (actionStr == "SELECTION_REQUEST") return ActionType::SELECTION_REQUEST;
    if (actionStr == "ERROR") return ActionType::ERROR;
    if (actionStr == "REACTION") return ActionType::REACTION;
    return ActionType::INVALID_ACTION;
}