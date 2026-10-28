#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <optional>
#include "shared/abstract/Card.hpp"
#include "shared/gameplay/Factory.hpp"

namespace Network {

    struct Payload {
        virtual QJsonObject toJson() const = 0;
    };

    struct PlayerState {
        QString playerName;
        int playerId;
        int playerScore;
        int hand;
        std::vector<const Card*> playedCards;
        int discardPile;
        const Card* topCardDiscardPile;
        int drawStack;

        PlayerState(const QString &playerName, int playerId, int playerScore, int hand, const std::vector<const Card*> &playedCards, int discardPile, const Card* topCardDiscardPile, int drawStack)
            : playerName(playerName), playerId(playerId), playerScore(playerScore), hand(hand), playedCards(playedCards), discardPile(discardPile), topCardDiscardPile(topCardDiscardPile), drawStack(drawStack) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["playerName"] = playerName;
            json["playerId"] = playerId;
            json["playerScore"] = playerScore;
            json["hand"] = hand;
            QJsonArray playedCardsArray;
            for (const auto &card : playedCards) {
                playedCardsArray.append(QString::fromStdString(card->getName()));
            }
            json["playedCards"] = playedCardsArray;
            json["discardPile"] = discardPile;
            json["topCardDiscardPile"] = topCardDiscardPile ? QString::fromStdString(topCardDiscardPile->getName()) : QString();
            json["drawStack"] = drawStack;
            return json;
        }

        static std::optional<PlayerState> fromJson(const QJsonObject &json) {
            if (!json.contains("playerName") || !json.contains("playerId") || !json.contains("playerScore") ||
                !json.contains("hand") || !json.contains("playedCards") || !json.contains("discardPile") ||
                !json.contains("topCardDiscardPile") || !json.contains("drawStack")) {
                return std::nullopt;
            }
            std::vector<const Card*> playedCards;
            for (const auto &cardJson : json["playedCards"].toArray()) {
                const Card* card = Factory::get(cardJson.toString().toStdString());
                if (card) {
                    playedCards.push_back(card);
                }
            }
            const Card* topCardDiscardPile = nullptr;
            if (!json["topCardDiscardPile"].toString().isEmpty()) {
                topCardDiscardPile = Factory::get(json["topCardDiscardPile"].toString().toStdString());
            }
            return PlayerState(
                json["playerName"].toString(),
                json["playerId"].toInt(),
                json["playerScore"].toInt(),
                json["hand"].toInt(),
                playedCards,
                json["discardPile"].toInt(),
                topCardDiscardPile,
                json["drawStack"].toInt()
            );
        }

    
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

    // The server sends this payload to all players in the room when a player plays a card.
    
    struct PlayCardServerPayload : public Payload {
        int roomId;
        int clientId;
        std::string phase;
        std::string selectionMode;
        int numberOfCardsToSelect;

        PlayCardServerPayload(int roomId, int clientId, const std::string &phase, const std::string &selectionMode, int numberOfCardsToSelect)
            : roomId(roomId), clientId(clientId), phase(phase), selectionMode(selectionMode), numberOfCardsToSelect(numberOfCardsToSelect) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["clientId"] = clientId;
            json["phase"] = QString::fromStdString(phase);
            json["selectionMode"] = QString::fromStdString(selectionMode);
            json["numberOfCardsToSelect"] = numberOfCardsToSelect;

            return json;
        }

        static std::optional<PlayCardServerPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("clientId") || !json.contains("phase") || !json.contains("selectionMode") || !json.contains("numberOfCardsToSelect")) {
                return std::nullopt;
            }
            return PlayCardServerPayload(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                json["phase"].toString().toStdString(),
                json["selectionMode"].toString().toStdString(),
                json["numberOfCardsToSelect"].toInt()
            );
        }
    };

    struct TurnStatePayload : public Payload {
        QString activePlayerName;
        QString phase;
        int actions;
        int buys;
        int money;

        TurnStatePayload(const QString &activePlayerName, const QString &phase, int actions, int buys, int money)
            : activePlayerName(activePlayerName), phase(phase), actions(actions), buys(buys), money(money) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["activePlayerName"] = activePlayerName;
            json["phase"] = phase;
            json["actions"] = actions;
            json["buys"] = buys;
            json["money"] = money;
            return json;
        }

        static std::optional<TurnStatePayload> fromJson(const QJsonObject &json) {
            if (!json.contains("activePlayerName") || !json.contains("phase") || !json.contains("actions") || !json.contains("buys") || !json.contains("money")) {
                return std::nullopt;
            }
            return TurnStatePayload(
                json["activePlayerName"].toString(),
                json["phase"].toString(),
                json["actions"].toInt(),
                json["buys"].toInt(),
                json["money"].toInt()
            );
        }
};

struct GameStatePayload : public Payload {
    QString activePlayerName;
    QString phase;
    int actions;
    int buys;
    int money;

    Hand hand;
    std::vector<const Card*> cardsInPlay;
    int drawStackCount;
    int discardStackCount;
    const Card* topCardDiscardPile;
    std::vector<PlayerState> otherPlayers;

    GameStatePayload( const QString &activePlayerName, const QString &phase, int actions, int buys, int money,
                     const Hand &hand, const std::vector<const Card*> &cardsInPlay, int drawStackCount, int discardStackCount,
                     const Card* topCardDiscardPile, const std::vector<PlayerState> &otherPlayers)
        :activePlayerName(activePlayerName), phase(phase), actions(actions), buys(buys), money(money),
          hand(hand), cardsInPlay(cardsInPlay), drawStackCount(drawStackCount), discardStackCount(discardStackCount),
          topCardDiscardPile(topCardDiscardPile), otherPlayers(otherPlayers) {}

    QJsonObject toJson() const {
        QJsonObject json;
        json["activePlayerName"] = activePlayerName;
        json["phase"] = phase;
        json["actions"] = actions;
        json["buys"] = buys;
        json["money"] = money;
        QJsonArray handArray;
        for (const auto &card : hand.getCards()) {
            handArray.append(QString::fromStdString(card->getName()));
        }
        json["hand"] = handArray;
        QJsonArray cardsInPlayArray;
        for (const auto &card : cardsInPlay) {
            cardsInPlayArray.append(QString::fromStdString(card->getName()));
        }
        json["cardsInPlay"] = cardsInPlayArray;
        json["drawStackCount"] = drawStackCount;
        json["discardStackCount"] = discardStackCount;
        json["topCardDiscardPile"] = topCardDiscardPile ? QString::fromStdString(topCardDiscardPile->getName()) : QString();
        QJsonArray otherPlayersArray;
        for (const auto &player : otherPlayers) {
            otherPlayersArray.append(player.toJson());
        }
        json["otherPlayers"] = otherPlayersArray;
        return json;
    }

    static std::optional<GameStatePayload> fromJson(const QJsonObject &json) {
        if (!json.contains("roomId") || !json.contains("activePlayerName") || !json.contains("phase") ||
            !json.contains("actions") || !json.contains("buys") || !json.contains("money") ||
            !json.contains("hand") || !json.contains("cardsInPlay") || !json.contains("drawStackCount") ||
            !json.contains("discardStackCount") || !json.contains("topCardDiscardPile") || !json.contains("otherPlayers")) {
            return std::nullopt;
        }

        std::vector<const Card*> cardsInPlay;
        for (const auto &cardJson : json["cardsInPlay"].toArray()) {
            cardsInPlay.push_back(Factory::get(cardJson.toString().toStdString()));
        }

        std::vector<PlayerState> otherPlayers;
        for (const auto &playerJson : json["otherPlayers"].toArray()) {
            auto playerStateOpt = PlayerState::fromJson(playerJson.toObject());
            if (playerStateOpt) {
                otherPlayers.push_back(*playerStateOpt);
            }
        }

        Hand hand;
        for (const auto &cardJson : json["hand"].toArray()) {
            hand.addCard(Factory::get(cardJson.toString().toStdString()));
        }

        return GameStatePayload(
            json["activePlayerName"].toString(),
            json["phase"].toString(),
            json["actions"].toInt(),
            json["buys"].toInt(),
            json["money"].toInt(),
            hand,
            cardsInPlay,
            json["drawStackCount"].toInt(),
            json["discardStackCount"].toInt(),
            Factory::get(json["topCardDiscardPile"].toString().toStdString()),
            otherPlayers
        );
    }
};


    // The client sends this payload to the server when he plays a card, it contains the roomId, the clientId and the selected cards.
    // The server will then validate the cards and send this payload back to all players in the room to let them know a card was played.
    struct CardsToBeInPlayPayload : public Payload {
        int roomId;
        int clientId;
        std::vector<const Card*> selectedCards;

        CardsToBeInPlayPayload(int roomId, int clientId, const std::vector<const Card*> &selectedCards)
            : roomId(roomId), clientId(clientId), selectedCards(selectedCards) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["clientId"] = clientId;  
            QJsonArray cardsArray;
            for (const auto &card : selectedCards) {
                cardsArray.append(card->getName().c_str());
            }
            json["selectedCards"] = cardsArray;
            return json;
        }

        static std::optional<CardsToBeInPlayPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("clientId") || !json.contains("selectedCards")) {
                return std::nullopt;
            }
            std::vector<const Card*> selectedCards;
            for (const auto &cardJson : json["selectedCards"].toArray()) {
                selectedCards.push_back(Factory::get(cardJson.toString().toStdString()));
            }
            return CardsToBeInPlayPayload(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                selectedCards
            );
        }
    };

    // Fired the server when a card was successfully played. Gives info regarding the card and the type it was used for (useful for double type cards like Witch)
    struct CardPlayed : public Payload {
        int roomId;
        int clientId;
        CardType type;
        const Card* card;

        CardPlayed(int roomId, int clientId, CardType t, const Card* c)
            : roomId(roomId), clientId(clientId), type(t), card(c) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["clientId"] = clientId;  
            json["type"] = cardTypeToString(type).c_str();
            json["card"] = card->getName().c_str();
            return json;
        }

        static std::optional<CardPlayed> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("clientId") || !json.contains("type") || !json.contains("card")) {
                return std::nullopt;
            }
            return CardPlayed(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                stringToCardType(json["type"].toString().toStdString()),
                Factory::get(json["card"].toString().toStdString())
            );
        }
    };

    // Fired the server when a card was successfully played. Gives info regarding the card and the type it was used for (useful for double type cards like Witch)
    struct AttackPlayed : public Payload {
        int roomId;
        int clientId;
        std::vector<QString> targets;
        const Card* card;

        AttackPlayed(int roomId, int clientId, std::vector<QString> t, const Card* c)
            : roomId(roomId), clientId(clientId), targets(t), card(c) {}
        
        AttackPlayed(int roomId, int clientId, std::vector<Player*> t, const Card* c)
            : roomId(roomId), clientId(clientId), card(c) {
                std::vector<QString> ts;
                for(Player* p : t){
                    ts.push_back(p->getName().c_str());
                }
                targets = ts;
            }

        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["clientId"] = clientId;  
            QJsonArray targs;
            for(QString n : targets){
                targs.append(n);
            }
            json["targets"] = targs;
            json["card"] = card->getName().c_str();
            return json;
        }

        static std::optional<AttackPlayed> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("clientId") || !json.contains("targets") || !json.contains("card")) {
                return std::nullopt;
            }

            std::vector<QString> ts;
            for(const auto t : json["targets"].toArray()){
                ts.push_back(t.toString());
            }

            return AttackPlayed(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                ts,
                Factory::get(json["card"].toString().toStdString())
            );
        }
    };

    // Fired the server when a card was successfully played. Gives info regarding the card and the type it was used for (useful for double type cards like Witch)
    struct Reaction : public Payload {
        int roomId;
        int clientId;
        QString attacker;
        const Card* card;
        bool protection;

        Reaction(int roomId, int clientId, QString at, const Card* c, bool p=false)
            : roomId(roomId), clientId(clientId), attacker(at), card(c), protection(p) {}
        
        Reaction(int roomId, int clientId, Player* at, const Card* c, bool p=false)
            : roomId(roomId), clientId(clientId), card(c), protection(p) {
                attacker = at->getName().c_str();
            }

        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["clientId"] = clientId;  
            json["attacker"] = attacker;
            json["card"] = card->getName().c_str();
            json["protection"] = protection? "TRUE" : "FALSE";
            return json;
        }

        static std::optional<Reaction> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("clientId") || !json.contains("attacker") || !json.contains("card")) {
                return std::nullopt;
            }

            return Reaction(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                json["attacker"].toString(),
                Factory::get(json["card"].toString().toStdString()),
                json.contains("protection") ? (json["protection"].toString().toStdString() == "TRUE" ? true : false) : false
            );
        }
    };

    // A payload sent by the server to notify the client that the requeted action ended up not working.
    struct ErrorPayload : public Payload {
        QString message;

        ErrorPayload(const QString &m)
            : message(m) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["message"] = message;
            return json;
        }

        static std::optional<ErrorPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("message")) {
                return std::nullopt;
            }
            return ErrorPayload(
                json["message"].toString()
            );
        }
    };
};