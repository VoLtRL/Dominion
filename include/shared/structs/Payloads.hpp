#pragma once
#include <QString>

namespace Network {

    // The client sends this payload to the server when a room is created.
    struct CreateRoomPayload {
        QString playerName;
        QString roomName;
        int botCount;
        int playerMaxCount;

        CreateRoomPayload(const QString &playerName, const QString &roomName, int botCount, int playerMaxCount)
            : playerName(playerName), roomName(roomName), botCount(botCount), playerMaxCount(playerMaxCount) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["playerName"] = playerName;
            json["roomName"] = roomName;
            json["botCount"] = botCount;
            json["playerMaxCount"] = playerMaxCount;
            return json;
        }

        static std::optional<CreateRoomPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("playerName") || !json.contains("roomName") || !json.contains("botCount") || !json.contains("playerMaxCount")) {
                return std::nullopt;
            }
            return CreateRoomPayload(
                json["playerName"].toString(),
                json["roomName"].toString(),
                json["botCount"].toInt(),
                json["playerMaxCount"].toInt()
            );
        }
    };

    // The server sends this payload to the client when a room is created.
    struct RoomJoinedPayload {

        int clientId;
        int roomId;
        QString roomName;
        int botCount;
        int playerMaxCount;

        RoomJoinedPayload(int clientId, int roomId, const QString &roomName, int botCount, int playerMaxCount)
            : clientId(clientId), roomId(roomId), roomName(roomName), botCount(botCount), playerMaxCount(playerMaxCount) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["roomName"] = roomName;
            json["botCount"] = botCount;
            json["playerMaxCount"] = playerMaxCount;
            json["clientId"] = clientId;
            json["roomId"] = roomId;
            return json;
        }

        static std::optional<RoomJoinedPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("clientId") || !json.contains("roomId") || !json.contains("roomName") || !json.contains("botCount") || !json.contains("playerMaxCount")) {
                return std::nullopt;
            }
            return RoomJoinedPayload(
                json["clientId"].toInt(),
                json["roomId"].toInt(),
                json["roomName"].toString(),
                json["botCount"].toInt(),
                json["playerMaxCount"].toInt()
            );
        }
    };

    // Used when a player wants to join a room, he sends his clientId and the roomId he wants to join
    struct JoinRoomPayload {
        int roomId;


        JoinRoomPayload(int roomId)
            : roomId(roomId) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            return json;
        }

        static std::optional<JoinRoomPayload> fromJson(const QJsonObject &json) {
            if ( !json.contains("roomId")) {
                return std::nullopt;
            }
            return JoinRoomPayload(
                json["roomId"].toInt()
            );
        }
    };

    // The server sends this payload to all players in the room when a new player joins the room
    struct PlayerJoinedRoomPayload {
    
        QString playerName;

        PlayerJoinedRoomPayload(const QString &playerName)
            : playerName(playerName) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["playerName"] = playerName;
            return json;
        }

        static std::optional<PlayerJoinedRoomPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("playerName")) {
                return std::nullopt;
            }
            return PlayerJoinedRoomPayload(
                json["playerName"].toString()
            );
        }

    };

    // The client uses this to tell the server he left a room.
    struct LeaveRoomPayload {
        QString roomName;

        LeaveRoomPayload(const QString &roomName)
            : roomName(roomName) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["roomName"] = roomName;
            return json;
        }

        static std::optional<LeaveRoomPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("roomName")) {
                return std::nullopt;
            }
            return LeaveRoomPayload(json["roomName"].toString());
        }
    };

    // The server send this to all of the players to let them know a player left their room.
    struct PlayerLeftRoom {
        QString playerName;

        PlayerLeftRoom(const QString &playerName)
            : playerName(playerName) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["playerName"] = playerName;
            return json;
        }

        static std::optional<PlayerLeftRoom> fromJson(const QJsonObject &json){
            if (!json.contains("playerName")){
                return std::nullopt;
            }
            return PlayerLeftRoom(json["playerName"].toString());
        }
    };

    struct StartGamePayload {
        QString roomName;

        StartGamePayload(const QString &roomName)
            : roomName(roomName) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["roomName"] = roomName;
            return json;
        }

        static std::optional<StartGamePayload> fromJson(const QJsonObject &json) {
            if (!json.contains("roomName")) {
                return std::nullopt;
            }
            return StartGamePayload(json["roomName"].toString());
        }
    };

    struct PlayCardPayload {
        int roomId;
        int clientId;
        int cardId;

        // Ajouter une phase pour spécifier l'action ? Globaliser ça à un vecteur de cartes plutot que d'avoir une seule carte à jouer à la fois ?

        PlayCardPayload(int roomId, int clientId, int cardId)
            : roomId(roomId), clientId(clientId), cardId(cardId) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["clientId"] = clientId;
            json["cardId"] = cardId;
            return json;
        }

        static std::optional<PlayCardPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("clientId") || !json.contains("cardId")) {
                return std::nullopt;
            }
            return PlayCardPayload(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                json["cardId"].toInt()
            );
        }
    };
};