#pragma once
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <optional>
#include "shared/abstract/Card.hpp"
#include "shared/gameplay/Factory.hpp"
#include "shared/gameplay/GameMode.hpp"
#include "shared/structs/EventType.hpp"

namespace Network {

    struct Payload {
        virtual QJsonObject toJson() const = 0;
    };

    struct PlayerState {
        QString playerName;

        int handSize;
        int discardStackSize;
        const Card* topDiscardCard;

        int drawStackSize;

        std::vector<const Card*> cardsInPlay;
        int drawStack;

        bool isActive;

        PlayerState(const QString &playerName, int handSize, int drawStackSize, int discardStackSize, const Card* topDiscardCard, std::vector<const Card*> cardsInPlay, bool isActive)
            : playerName(playerName), handSize(handSize), drawStackSize(drawStackSize), discardStackSize(discardStackSize), topDiscardCard(topDiscardCard), cardsInPlay(cardsInPlay), isActive(isActive) {}

        QJsonObject toJson() const {
            QJsonObject json;
            json["playerName"] = playerName;
            json["handSize"] = handSize;
            json["drawStackSize"] = drawStackSize;
            json["discardStackSize"] = discardStackSize;
            if (topDiscardCard) {
                json["topDiscardCard"] = QString::fromStdString(topDiscardCard->getName());
            } else {
                json["topDiscardCard"] = QJsonValue::Null;
            }
            QJsonArray cardsArray;
            for (const auto &card : cardsInPlay) {
                cardsArray.append(QString::fromStdString(card->getName()));
            }
            json["cardsInPlay"] = cardsArray;
            json["isActive"] = isActive;
            return json;
        }

        static std::optional<PlayerState> fromJson(const QJsonObject &json) {
            if (!json.contains("playerName") || !json.contains("handSize") || !json.contains("drawStackSize") || !json.contains("discardStackSize") || !json.contains("topDiscardCard") || !json.contains("cardsInPlay") || !json.contains("isActive")) {
                return std::nullopt;
            }
            std::vector<const Card*> cardsInPlay;
            for (const auto &cardJson : json["cardsInPlay"].toArray()) {
                const Card* card = Factory::get(cardJson.toString().toStdString());
                if (card) {
                    cardsInPlay.push_back(card);
                }
            }
            const Card* topDiscardCard = nullptr;
            if (!json["topDiscardCard"].isNull()) {
                topDiscardCard = Factory::get(json["topDiscardCard"].toString().toStdString());
            }
            return PlayerState(
                json["playerName"].toString(),
                json["handSize"].toInt(),
                json["drawStackSize"].toInt(),
                json["discardStackSize"].toInt(),
                topDiscardCard,
                cardsInPlay,
                json["isActive"].toBool()
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
        GameMode gameMode;
        int botCount;
        int playerMaxCount;

        CreateRoomPayload(const QString &playerName, const QString &roomName, const GameMode &gameMode, int botCount, int playerMaxCount)
            : playerName(playerName), roomName(roomName), gameMode(gameMode), botCount(botCount), playerMaxCount(playerMaxCount) {}
        
        QJsonObject toJson() const {
            QJsonObject json;
            json["playerName"] = playerName;
            json["roomName"] = roomName;
            json["gameMode"] = gameMode.toJson();
            json["botCount"] = botCount;
            json["playerMaxCount"] = playerMaxCount;
            return json;
        }

        static std::optional<CreateRoomPayload> fromJson(const QJsonObject &json) {
            if (!json.contains("playerName") || !json.contains("roomName") || !json.contains("botCount") || !json.contains("playerMaxCount")) {
                return std::nullopt;
            }
            auto gameModeOpt = GameMode::fromJson(json["gameMode"].toObject());
            if (!gameModeOpt) {
                return std::nullopt;
            }
            return CreateRoomPayload(
                json["playerName"].toString(),
                json["roomName"].toString(),
                gameModeOpt.value(),
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
    QString selfName;
    QString currentPhase;

    int actions;
    int buys;
    int money;

    Hand hand;
    std::vector<const Card*> cardsInPlay;

    std::map<const Card*, int> cardsInShop;

    int drawStackCount;
    int discardStackCount;
    const Card* topCardDiscardPile;
    std::vector<PlayerState> otherPlayers;

    GameStatePayload(const QString &selfName, const QString &currentPhase, int actions, int buys, int money, const Hand &hand, const std::vector<const Card*> &cardsInPlay, std::map<const Card*, int> cardsInShop, int drawStackCount, int discardStackCount, const Card* topCardDiscardPile, const std::vector<PlayerState> &otherPlayers)
        : selfName(selfName), currentPhase(currentPhase), actions(actions), buys(buys), money(money), hand(hand), cardsInPlay(cardsInPlay), cardsInShop(cardsInShop), drawStackCount(drawStackCount), discardStackCount(discardStackCount), topCardDiscardPile(topCardDiscardPile), otherPlayers(otherPlayers) {}

    QJsonObject toJson() const {
        QJsonObject json;
        json["selfName"] = selfName;
        json["currentPhase"] = currentPhase;
        json["actions"] = actions;
        json["buys"] = buys;
        json["money"] = money;
        QJsonArray handJson;
        for (const auto &card : hand.getCards()) {
            handJson.append(QString::fromStdString(card->getName()));
        }
        json["hand"] = handJson;
        QJsonArray cardsInPlayArray;
        for (const auto &card : cardsInPlay) {
            cardsInPlayArray.append(card->getName().c_str());
        }
        QJsonObject shopJson;
        for (const auto &pair : cardsInShop) {
            shopJson.insert(pair.first->getName().c_str(), pair.second);
        }
        json["shop"] = shopJson;
        json["cardsInPlay"] = cardsInPlayArray;
        json["drawStackCount"] = drawStackCount;
        json["discardStackCount"] = discardStackCount;
        json["topCardDiscardPile"] = topCardDiscardPile ? topCardDiscardPile->getName().c_str() : QString();
        QJsonArray otherPlayersArray;
        for (const auto &playerState : otherPlayers) {
            otherPlayersArray.append(playerState.toJson());
        }
        json["otherPlayers"] = otherPlayersArray;
        return json;
    }

    static std::optional<GameStatePayload> fromJson(const QJsonObject &json) {
        if (!json.contains("selfName") || !json.contains("currentPhase") || !json.contains("actions") || !json.contains("buys") || !json.contains("money") || !json.contains("hand") || !json.contains("cardsInPlay") || !json.contains("shop") || !json.contains("drawStackCount") || !json.contains("discardStackCount") || !json.contains("topCardDiscardPile") || !json.contains("otherPlayers")) {
            return std::nullopt;
        }
        Hand hand;
        for (const auto &cardJson : json["hand"].toArray()) {
            const Card *card = Factory::get(cardJson.toString().toStdString());
            if (card) {
                hand.addCard(card);
            }
        }
        std::vector<const Card*> cardsInPlay;
        for (const auto &cardJson : json["cardsInPlay"].toArray()) {
            cardsInPlay.push_back(Factory::get(cardJson.toString().toStdString()));
        }

        std::map<const Card*, int> shop;
        for (const auto &key : json["shop"].toObject().keys()) {
            const Card *card = Factory::get(key.toStdString());
            if (card) {
                shop[card] = json["shop"].toObject()[key].toInt();
            }
        }

        const Card* topCardDiscardPile = nullptr;
        if (!json["topCardDiscardPile"].toString().isEmpty()) {
            topCardDiscardPile = Factory::get(json["topCardDiscardPile"].toString().toStdString());
        }
        std::vector<PlayerState> otherPlayers;
        for (const auto &playerStateJson : json["otherPlayers"].toArray()) {
            auto playerStateOpt = PlayerState::fromJson(playerStateJson.toObject());
            if (playerStateOpt) {
                otherPlayers.push_back(*playerStateOpt);
            }
        }
        return GameStatePayload(
            json["selfName"].toString(),
            json["currentPhase"].toString(),
            json["actions"].toInt(),
            json["buys"].toInt(),
            json["money"].toInt(),
            hand,
            cardsInPlay,
            shop,
            json["drawStackCount"].toInt(),
            json["discardStackCount"].toInt(),
            topCardDiscardPile,
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

    // Fired by the server when a card was successfully played. Gives info regarding the card and the type it was used for (useful for double type cards like Witch)
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

    // Fired by the server when an attack card was successfully played.
    struct AttackPlayed : public Payload {
        int roomId;
        int clientId;
        std::set<QString> targets;
        const Card* card;

        AttackPlayed(int roomId, int clientId, std::set<QString> t, const Card* c)
            : roomId(roomId), clientId(clientId), targets(t), card(c) {}
        
        AttackPlayed(int roomId, int clientId, std::set<Player*> t, const Card* c)
            : roomId(roomId), clientId(clientId), card(c) {
                std::set<QString> ts;
                for(Player* p : t){
                    ts.insert(p->getName().c_str());
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

            std::set<QString> ts;
            for(const auto t : json["targets"].toArray()){
                ts.insert(t.toString());
            }

            return AttackPlayed(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                ts,
                Factory::get(json["card"].toString().toStdString())
            );
        }
    };

    // Fired by the client when a Reaction Card to an action is played.
    // Fired by the server once the action is valid.
    struct ReactionPlayed : public Payload {
        int roomId;
        int clientId;
        QString actorPlayer;
        const Card* reactionCard;
        EventType type;

        ReactionPlayed(int roomId, int clientId, QString at, const Card* c, EventType t)
            : roomId(roomId), clientId(clientId), actorPlayer(at), reactionCard(c), type(t) {}
        
        ReactionPlayed(int roomId, int clientId, Player* at, const Card* c, EventType t)
            : roomId(roomId), clientId(clientId), reactionCard(c), type(t) {
                actorPlayer = at->getName().c_str();
            }

        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["clientId"] = clientId;  
            json["actorPlayer"] = actorPlayer;
            json["reactionCard"] = reactionCard->getName().c_str();
            json["type"] = eventTypeToString(type).c_str();
            return json;
        }

        static std::optional<ReactionPlayed> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("clientId") || !json.contains("actorPlayer") || !json.contains("card") || !json.contains("type")) {
                return std::nullopt;
            }

            return ReactionPlayed(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                json["actorPlayer"].toString(),
                Factory::get(json["reactionCard"].toString().toStdString()),
                stringToEventType(json["type"].toString().toStdString())
            );
        }
    };

    // Fired by the server to clients that can react to a specific attack.
    struct ReactionNeeded : public Payload {
        int roomId;
        int clientId;
        QString actorPlayer;
        EventType type;

        ReactionNeeded(int roomId, int clientId, QString at, EventType t)
            : roomId(roomId), clientId(clientId), actorPlayer(at), type(t) {}
        
        ReactionNeeded(int roomId, int clientId, Player* at, EventType t)
            : roomId(roomId), clientId(clientId), type(t) {
                actorPlayer = at->getName().c_str();
            }

        QJsonObject toJson() const {
            QJsonObject json;
            json["roomId"] = roomId;
            json["clientId"] = clientId;  
            json["actorPlayer"] = actorPlayer;
            json["type"] = eventTypeToString(type).c_str();
            return json;
        }

        static std::optional<ReactionNeeded> fromJson(const QJsonObject &json) {
            if (!json.contains("roomId") || !json.contains("clientId") || !json.contains("actorPlayer") || !json.contains("type")) {
                return std::nullopt;
            }

            return ReactionNeeded(
                json["roomId"].toInt(),
                json["clientId"].toInt(),
                json["actorPlayer"].toString(),
                stringToEventType(json["type"].toString().toStdString())
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