#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <optional>

namespace Network {

    struct Payload {
        virtual QJsonObject toJson() const = 0;
    };


    struct RoomInfo {
            int roomId;
            QString roomName;
            int placesLeft;
            std::vector<QString> playerNames;
            RoomInfo(int roomId, const QString &roomName,std::vector<QString> playerNames, int placesLeft)
                : roomId(roomId), roomName(roomName), playerNames(playerNames), placesLeft(placesLeft) {}
            QJsonObject toJson() const {
                QJsonObject json;
                json["roomId"] = roomId;
                json["roomName"] = roomName;
                QJsonArray playersArray;
                for (const auto &playerName : playerNames) {
                    playersArray.append(playerName);
                }
                json["playerNames"] = playersArray;
                json["placesLeft"] = placesLeft;
                return json;
            }
            static std::optional<RoomInfo> fromJson(const QJsonObject &json) {
                if (!json.contains("roomId") || !json.contains("roomName") || !json.contains("playerNames") || !json.contains("placesLeft")) {
                    return std::nullopt;
                }
                return RoomInfo(
                    json["roomId"].toInt(),
                    json["roomName"].toString(),
                    [json]() {
                        std::vector<QString> playerNames;
                        for (const auto &playerNameJson : json["playerNames"].toArray()) {
                            playerNames.push_back(playerNameJson.toString());
                        }
                        return playerNames;
                    }(),
                    json["placesLeft"].toInt()
                );
            }
        };

    // The client sends this payload to the server when a room is created.
    struct CreateRoomPayload : public Payload {
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

    struct CreatedRoomPayload : public Payload {
        int roomId;
        QString roomName;

        CreatedRoomPayload(int roomId, const QString &roomName)
            : roomId(roomId), roomName(roomName) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["roomName"] = roomName;
            return json;
        }

        static std::optional<CreatedRoomPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("roomName")) {
                return std::nullopt;
            }
            return CreatedRoomPayload(
                json["roomId"].toInt(),
                json["roomName"].toString()
            );
        }
    };

    // The server sends this payload to the client when he successfully joined a room, it contains the roomId and the clientId of the player.
    struct RoomJoinedPayload : public Payload {

        Network::RoomInfo roomInfo;
        std::vector<QString> playerNames;


        RoomJoinedPayload(const Network::RoomInfo &roomInfo, const std::vector<QString> &playerNames)
            : roomInfo(roomInfo), playerNames(playerNames) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["roomInfo"] = roomInfo.toJson();
            QJsonArray playersArray;
            for (const auto &playerName : playerNames) {
                playersArray.append(playerName);
            }
            json["playerNames"] = playersArray;
            return json;
        }

        static std::optional<RoomJoinedPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("roomInfo") || !json.contains("playerNames")) {
                return std::nullopt;
            }
            auto roomInfoOpt = RoomInfo::fromJson(json["roomInfo"].toObject());
            if (!roomInfoOpt) {
                return std::nullopt;
            }
            std::vector<QString> playerNames;
            for (const auto &playerNameJson : json["playerNames"].toArray()) {
                playerNames.push_back(playerNameJson.toString());
            }
            return RoomJoinedPayload(*roomInfoOpt, playerNames);
        }
    };

    // The client sends this payload when he wants something from the server, like the list of rooms available to join.
    struct EmptyPayload : public Payload {
        QJsonObject toJson() const {
            return QJsonObject();
        }

        static std::optional<EmptyPayload> fromJson(const QJsonObject &json) {
            return EmptyPayload();
        }
    };

    // The server sends this payload to the client when he requests the list of rooms available to join.
    struct RoomListPayload : public Payload {
        std::vector<RoomInfo> rooms;

        RoomListPayload(const std::vector<RoomInfo> &rooms)
            : rooms(rooms) {}

        QJsonObject toJson() const {
            QJsonObject json;
            QJsonArray roomsArray;
            for (const auto &room : rooms) {
                roomsArray.append(room.toJson());
            }
            json["rooms"] = roomsArray;
            return json;
        }

        static std::optional<RoomListPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("rooms")) {
                return std::nullopt;
            }
            std::vector<RoomInfo> rooms;
            for (const auto &roomJson : json["rooms"].toArray()) {
                auto room = RoomInfo::fromJson(roomJson.toObject());
                if (room) {
                    rooms.push_back(*room);
                }
            }
            return RoomListPayload(rooms);
        }

    };

    // Used when a player wants to join a room, he sends his clientId and the roomId he wants to join
    struct JoinRoomPayload : public Payload {
        int roomId;
        QString playerName;

        JoinRoomPayload(int roomId, const QString &playerName)
            : roomId(roomId), playerName(playerName) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["playerName"] = playerName;
            return json;
        }

        static std::optional<JoinRoomPayload> fromJson(const QJsonObject &json) {
            if ( !json.contains("roomId") || !json.contains("playerName")) {
                return std::nullopt;
            }
            return JoinRoomPayload(
                json["roomId"].toInt(),
                json["playerName"].toString()
            );
        }
    };

    // The server sends this payload to all players in the room when a new player joins the room
    struct PlayerJoinedRoomPayload : public Payload {
    
        QString playerName;

        PlayerJoinedRoomPayload(const QString &playerName)
            : playerName(playerName) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["playerName"] = playerName;
            return json;
        }

        static std::optional<PlayerJoinedRoomPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("playerName")) {
                return std::nullopt;
            }
            return PlayerJoinedRoomPayload(
                json["playerName"].toString()
            );
        }

    };

    // The client uses this to tell the server he left a room.
    struct LeaveRoomPayload : public Payload{
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
    struct PlayerLeftRoomPayload : public Payload{
        QString playerName;

        PlayerLeftRoomPayload(const QString &playerName)
            : playerName(playerName) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["playerName"] = playerName;
            return json;
        }

        static std::optional<PlayerLeftRoomPayload> fromJson(const QJsonObject &json){
            if (!json.contains("playerName")){
                return std::nullopt;
            }
            return PlayerLeftRoomPayload(json["playerName"].toString());
        }
    };

    struct StartGamePayload : public Payload {
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

    struct PlayCardPayload : public Payload {
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