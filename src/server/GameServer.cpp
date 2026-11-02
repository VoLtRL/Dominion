#include "server/GameServer.hpp"
#include "server/Client.hpp"
#include "shared/structs/ActionType.hpp"
#include "shared/abstract/Action.hpp"
#include "shared/abstract/Attack.hpp"
#include "shared/cards/Library.hpp"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <iostream>

GameServer::GameServer(QObject *parent) : QObject(parent) {
    connect(&m_server, &QTcpServer::newConnection, this, [this]() {
        while (m_server.hasPendingConnections()) {
            std::cout << "New client connected." << std::endl;
            QTcpSocket *clientSocket = m_server.nextPendingConnection();
            Client *newClient = new Client(clientSocket);
            m_clients.insert({clientSocket, newClient});

            connect(clientSocket, &QTcpSocket::readyRead, this, [this, clientSocket]() {
                while (clientSocket->canReadLine()) {
                    QByteArray data = clientSocket->readLine().trimmed();
                    if (data.isEmpty()) continue;

                    QJsonParseError parseError;
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(data, &parseError);

                    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
                        qWarning() << "Invalid JSON received :" << parseError.errorString();
                        continue;
                    }

                    QJsonObject jsonObj = jsonDoc.object();
                    QString action = jsonObj.value("action").toString();
                    QJsonObject actionData = jsonObj.value("data").toObject();
                    receiveAction(action, actionData, clientSocket);
                }
            });

            connect(clientSocket, &QTcpSocket::disconnected, this, [this, clientSocket]() {
                auto it = m_clients.find(clientSocket);
                if (it != m_clients.end()) {
                    Client *client = it->second;
                    m_clients.erase(it);
                    RoomManager::getInstance().leaveRoom(client);
                    delete client;
                }
                clientSocket->deleteLater();
            });
        }
    });
}

void GameServer::startServer(quint16 port) {
    if (!m_server.listen(QHostAddress::Any, port)) {
        qWarning() << "Failed to start server on port" << port << ":" << m_server.errorString();
    } else {
        qDebug() << "Server started on port" << port;
    }
}

void GameServer::stopServer() {
    m_server.close();
    qDebug() << "Server stopped.";
}

Client* GameServer::getClientById(int id){
    for(auto pair : m_clients){
        if(pair.second->getId() == id){
            return pair.second;
        }
    }
    return nullptr;
}

void GameServer::receiveAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket) {
    auto clientIt = m_clients.find(clientSocket);
    if (clientIt == m_clients.end() || !clientIt->second) {
        qWarning() << "Received action from unknown client.";
        return;
    }
    Client *client = clientIt->second;

    std::cout << "Received action: " << action.toStdString() << " from client ID: "<< client->getId() << std::endl;

    switch (stringToActionType(action.toStdString())) {
        case ActionType::CREATE_ROOM:
            handleCreateRoom(data, client);
            break;
        case ActionType::JOIN_ROOM:
            handleJoinRoom(data, client);
            break;
        case ActionType::REQUEST_ROOM_LIST:
            handleRequestRoomList(client);
            break;
        case ActionType::LEAVE_ROOM:
            handleLeaveRoom(client);
            break;
        case ActionType::START_GAME:
            handleStartGame(client);
            break;
        case ActionType::PLAY_CARD:
            handlePlayCard(data, client);
            break;
        case ActionType::PLAY_ALL_TREASURES:
            handlePlayAllTreasures(client);
            break;
        case ActionType::BUY_CARD:
            handleBuyCard(data, client);
            break;
        case ActionType::NEXT_PHASE:
            handleNextPhase(client);
            break;
        case ActionType::END_TURN:
            handleEndTurn(client);
            break;
        case ActionType::SELECTION_REQUEST:
            handleSelection(data, client);
            break;
        case ActionType::REACTION:
            handleReaction(data, client);
            break;
        default:
            qWarning() << "Received invalid action:" << action;
            break;
    }
}

void GameServer::sendGameStateToRoom(Room *room) {
    for (Client *roomClient : room->getClients()) {
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::GAME_STATE)),
            room->getGameStatePayload(roomClient).toJson(),
            roomClient->getSocket());
    }
}

void GameServer::handleCreateRoom(const QJsonObject &data, Client *client) {
    auto payload = Network::CreateRoomPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid data given" << data;
        return;
    }

    auto &roomMgr = RoomManager::getInstance();
    client->setName(payload->playerName);
    roomMgr.createRoom(*payload, client);

    auto room = roomMgr.getRoomByClient(client);
    if (!room) {
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::ROOM_CREATION_FAILED)),
            QJsonObject{{"error", "Room creation failed."}},
            client->getSocket());
        return;
    }

    room->setGameMode(payload->gameMode);

    Network::CreatedRoomPayload roomCreated(
        roomMgr.getRoomByClient(client)->getId(),
        payload->roomName);
        sendAction(
        QString::fromStdString(actionTypeToString(ActionType::ROOM_CREATED)),
        roomCreated.toJson(),
        client->getSocket());
}

void GameServer::handleJoinRoom(const QJsonObject &data, Client *client) {
    auto payload = Network::JoinRoomPayload::fromJson(data);
    if (!payload) {
        qWarning() << "Invalid data given" << data;
        return;
    }

    auto &roomMgr = RoomManager::getInstance();
    client->setName(payload->playerName);
    Room *targetRoom = roomMgr.getRoomById(payload->roomId);
    if (!targetRoom || !roomMgr.joinRoom(client, targetRoom)) {
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::ROOM_JOIN_FAILED)),
            QJsonObject{{"error", "Room is unavailable."}},
            client->getSocket());
        return;
    }

    std::vector<QString> playerNames;
    for (const Client *roomClient : targetRoom->getClients()) {
        playerNames.push_back(roomClient->getName());
    }
    Network::RoomInfo roomInfo(
        targetRoom->getId(),
        targetRoom->getName(),
        playerNames,
        targetRoom->getPlacesLeft());
    Network::RoomJoinedPayload roomJoined(roomInfo, playerNames);
    sendAction(
        QString::fromStdString(actionTypeToString(ActionType::JOIN_ROOM)),
        roomJoined.toJson(),
        client->getSocket());

    Network::PlayerJoinedRoomPayload playerJoined(client->getName());
    for (Client *roomClient : targetRoom->getClients()) {
        if (roomClient != client) {
            sendAction(
                QString::fromStdString(actionTypeToString(ActionType::PLAYER_JOINED)),
                playerJoined.toJson(),
                roomClient->getSocket());
        }
    }
}

void GameServer::handleNextPhase(Client *client) {
    RoomManager &roomMgr = RoomManager::getInstance();
    Room *room = roomMgr.getRoomByClient(client);
    if (!room || !room->getGame()) {
        return;
    }

    Game *game = room->getGame();
    if (!game->isPlayerTurn(client->getAssociatedPlayer())) {
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::ERROR)),
            Network::ErrorPayload("It's not your turn.").toJson(),
            client->getSocket());
        return;
    }

    if(!game->tryNextPhase(client->getAssociatedPlayer())) {
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::ERROR)),
            Network::ErrorPayload("Failed to advance to the next phase.").toJson(),
            client->getSocket());
        return;
    }
    for (Client *roomClient : room->getClients()) {
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::NEXT_PHASE)),
            Network::PhaseChangedPayload(game->getCurrentPhase()).toJson(),
            roomClient->getSocket());
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::GAME_STATE)),
            room->getGameStatePayload(roomClient).toJson(),
            roomClient->getSocket());
    }
}

void GameServer::handleRequestRoomList(Client *client) {
    auto &roomMgr = RoomManager::getInstance();
    Network::RoomListPayload roomList({});
    for (const Room *room : roomMgr.getAvailableRooms()) {
        std::vector<QString> playerNames;
        for (const Client *roomClient : room->getClients()) {
            playerNames.push_back(roomClient->getName());
        }
        roomList.rooms.push_back(Network::RoomInfo(
            room->getId(),
            room->getName(),
            playerNames,
            room->getPlacesLeft()));
    }

    sendAction(
        QString::fromStdString(actionTypeToString(ActionType::REQUEST_ROOM_LIST)),
        roomList.toJson(),
        client->getSocket());
}

void GameServer::handleLeaveRoom(Client *client) {
    RoomManager &roomMgr = RoomManager::getInstance();
    Room *room = roomMgr.getRoomByClient(client);
    std::cout << "Client ID: " << client->getId() << " is leaving room." << std::endl;

    if (!room) {
        return;
    }

    Network::PlayerLeftRoomPayload playerLeft(client->getName());
    for (Client *roomClient : room->getClients()) {
        if (roomClient != client) {
            sendAction(
                QString::fromStdString(actionTypeToString(ActionType::PLAYER_LEFT)),
                playerLeft.toJson(),
                roomClient->getSocket());
        }
    }

    roomMgr.leaveRoom(client);
}

void GameServer::handleStartGame(Client *client) {
    RoomManager &roomMgr = RoomManager::getInstance();
    Room *room = roomMgr.getRoomByClient(client);
    if (!room || !roomMgr.startGame(client)) {
        return;
    }

    Network::StartGamePayload gameStarted(room->getName());
    for (Client *roomClient : room->getClients()) {
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::GAME_STATE)),
            room->getGameStatePayload(roomClient).toJson(),
            roomClient->getSocket());
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::START_GAME)),
            gameStarted.toJson(),
            roomClient->getSocket());
    }
}

void GameServer::handlePlayCard(const QJsonObject &data, Client *client) {
    if (Room *room = RoomManager::getInstance().getRoomByClient(client)) {
        if (Game *game = room->getGame()) {
            auto payload = Network::PlayCardPayload::fromJson(data);
            if (!payload) {
                qWarning() << "Invalid data given" << data;
                return;
            }

            const Card* cardToPlay = payload->cardToPlay;
            if (!cardToPlay) {
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                    Network::ErrorPayload("Unknown card.").toJson(),
                    client->getSocket()
                );
                return;
            }

                if(cardToPlay->isA(CardType::ACTION)) {
                    if(!game->tryPlayAction(client->getAssociatedPlayer(), cardToPlay)){
                        const std::string message = "ACTION CARD ERROR : " + cardToPlay->getName() + " FROM PLAYER " + client->getAssociatedPlayer()->getName();
                        sendAction(
                            QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                            Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                            client->getSocket()
                        );
                        return;
                    }
                    game->setPendingCard(cardToPlay);
                    game->setPendingActor(client->getAssociatedPlayer());
                    if (cardToPlay->getName() == "Library") {
                        game->clearPendingLibrarySkipped();
                    }

                    for(Client* cl : room->getClients()){
                        sendAction(
                            QString::fromStdString(actionTypeToString(ActionType::PLAY_CARD)),
                            Network::CardPlayed(
                                CardType::ACTION,
                                cardToPlay
                            ).toJson(),
                            cl->getSocket()
                        );
                    }
                    

                    if(cardToPlay->isA(CardType::ATTACK)){
                        if(!game->canPlayAttack(client->getAssociatedPlayer(), cardToPlay)){
                            const std::string message = "ATTACK CARD ERROR : " + cardToPlay->getName() + " FROM PLAYER " + client->getAssociatedPlayer()->getName();
                            sendAction(
                                QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                                Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                                client->getSocket()
                            );
                            return;
                        }
                        std::set<Player*> targets;
                        for(Player &p : game->getPlayers()) {
                            targets.insert(&p);
                        }
                        game->setCurrentTargets(targets);
                        game->setPendingCard(cardToPlay);
                        game->setPendingActor(client->getAssociatedPlayer());
                        if (cardToPlay->getName() == "Thief") {
                            game->clearPendingThiefTreasures();
                        }
                        std::vector<Player*> canReact;
                        for(Player* target : targets) {
                            if(target == client->getAssociatedPlayer()){
                                if(target->canReactToEvent(EventType::ATTACK_PLAYED_SELF)) { // revoir pour la militia par exemple
                                    canReact.push_back(target);
                                }
                            }
                            else {
                                if(target->canReactToEvent(EventType::ATTACK_PLAYED_OTHER)) {
                                    canReact.push_back(target);
                                }
                            }
                        }

                        if(canReact.empty()){
                            if(!game->tryPlayAttack(client->getAssociatedPlayer(), cardToPlay)){
                                const std::string message = "ATTACK CARD ERROR : " + cardToPlay->getName() + " FROM PLAYER " + client->getAssociatedPlayer()->getName();
                                sendAction(
                                    QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                                    Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                                    client->getSocket()
                                );
                                return;
                            }

                            for(Client* cl : room->getClients()){
                                sendAction(
                                    QString::fromStdString(actionTypeToString(ActionType::PLAY_CARD)),
                                    Network::AttackPlayed(
                                        targets,
                                        cardToPlay
                                    ).toJson(),
                                    cl->getSocket()
                                );
                            }

                            sendGameStateToRoom(room);
                            sendSelectionRequest(room, game);
                            return;
                        }

                        game->setPendingEvent(EventType::ATTACK_PLAYED_SELF);
                        game->setPendingPlayers(canReact);
                        game->setCurrentType(CardType::ATTACK);

                        for(Client* cl : room->getClients()){
                            if(std::find(canReact.begin(), canReact.end(), cl->getAssociatedPlayer()) != canReact.end()){
                                sendAction(
                                    QString::fromStdString(actionTypeToString(ActionType::REACTION)),
                                    Network::ReactionPayload(
                                        cl->getAssociatedPlayer()->getName().c_str(),
                                        cardToPlay,
                                        cl->getAssociatedPlayer()->getCardsInHandOfType(CardType::REACTION)
                                    ).toJson(),
                                    cl->getSocket()
                                );
                            }
                        }
                    }
                    sendGameStateToRoom(room);
                    sendSelectionRequest(room, game);
                    return;
                }
                else if(cardToPlay->isA(CardType::TREASURE)) {
                    if(!game->tryPlayTreasure(client->getAssociatedPlayer(), cardToPlay)){
                        const std::string message = "TREASURE CARD ERROR : " + cardToPlay->getName() + " FROM PLAYER " + client->getAssociatedPlayer()->getName();
                        sendAction(
                            QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                            Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                            client->getSocket()
                        );
                        return;
                    }
                    for(Client* cl : room->getClients()) {
                        sendAction(
                        QString::fromStdString(actionTypeToString(ActionType::PLAY_CARD)),
                        Network::CardPlayed(
                            CardType::TREASURE,
                            cardToPlay
                        ).toJson(),
                        cl->getSocket()
                    );
                    }
                sendGameStateToRoom(room);
                sendSelectionRequest(room, game);
                }
                else {
                    const std::string message = "PLAY CARD ERROR : " + cardToPlay->getName() + " FROM PLAYER " + client->getAssociatedPlayer()->getName();
                    sendAction(
                        QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                        Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                        client->getSocket()
                    );
                    return;
                }
            }

        }
    }

void GameServer::sendSelectionRequest(Room *room, Game *game) {
    if (!room || !game || !game->hasPendingSelection()) {
        return;
    }

    for (Client *roomClient : room->getClients()) {
        if (roomClient->getAssociatedPlayer() == game->getSelectionPlayer()) {
            sendAction(QString::fromStdString(actionTypeToString(ActionType::SELECTION_REQUEST)),Network::SelectionRequestPayload(
                    game->getSelectionMode(),
                    game->getSelectionConstraint(),
                    static_cast<int>(game->getSelectionCount()),
                    game->getSelectionCards()).toJson(),
                roomClient->getSocket());
            return;
        }
    }
}

void GameServer::handleSelection(const QJsonObject &data, Client *client) {
    Room *room = RoomManager::getInstance().getRoomByClient(client);
    if (!room || !room->getGame()) {
        return;
    }

    Game *game = room->getGame();
    auto payload = Network::SelectionResponsePayload::fromJson(data);
    if (!payload || !game->hasPendingSelection() || client->getAssociatedPlayer() != game->getSelectionPlayer() || !game->trySendSelection(client->getAssociatedPlayer(), payload->cards)) {
        sendAction( QString::fromStdString(actionTypeToString(ActionType::ERROR)), Network::ErrorPayload("Invalid card selection.").toJson(), client->getSocket());
        return;
    }

    const auto selected = payload->cards;
    const auto offered = game->getSelectionCards();
    const auto context = game->getSelectionContext();
    Player *player = game->getSelectionPlayer();
    game->clearSelection();

    switch (context) {
        case Game::SelectionContext::WORKSHOP_GAIN:
            if (!selected.empty()) {
                player->addCardToDiscardStack(game->getCardFromCardDistrib(selected.front()->getName()));
            }
            break;
        case Game::SelectionContext::CHAPEL_TRASH:
            for (const Card *card : selected) {
                player->removeCardFromHand(card);
                game->trashCard(card);
            }
            break;
        case Game::SelectionContext::CELLAR_DISCARD:
            for (const Card *card : selected) {
                player->discardCard(card);
            }
            player->drawCardsToHand(static_cast<int>(selected.size()));
            break;
        case Game::SelectionContext::REMODEL_TRASH:
            if (!selected.empty()) {
                const int maxCost = selected.front()->getCost() + 2;
                player->removeCardFromHand(selected.front());
                game->trashCard(selected.front());
                game->requestSelection(
                    player, game->getBuyableCardsBelowCost(maxCost), 1,
                    SelectionMode::GAIN, SelectionConstraint::EXACTLY,
                    Game::SelectionContext::REMODEL_GAIN);
                sendGameStateToRoom(room);
                sendSelectionRequest(room, game);
                return;
            }
            break;
        case Game::SelectionContext::REMODEL_GAIN:
            if (!selected.empty()) {
                player->addCardToDiscardStack(game->getCardFromCardDistrib(selected.front()->getName()));
            }
            break;
        case Game::SelectionContext::MINE_TRASH:
            if (!selected.empty()) {
                const int maxCost = selected.front()->getCost() + 3;
                player->removeCardFromHand(selected.front());
                game->trashCard(selected.front());
                std::vector<const Card*> treasures;
                for (const Card *card : game->getBuyableCardsBelowCost(maxCost)) {
                    if (card->isA(CardType::TREASURE)) {
                        treasures.push_back(card);
                    }
                }
                game->requestSelection(
                    player, treasures, 1,
                    SelectionMode::GAIN, SelectionConstraint::EXACTLY,
                    Game::SelectionContext::MINE_GAIN);
                sendGameStateToRoom(room);
                sendSelectionRequest(room, game);
                return;
            }
            break;
        case Game::SelectionContext::MINE_GAIN:
            if (!selected.empty()) {
                player->addCardToHand(game->getCardFromCardDistrib(selected.front()->getName()));
            }
            break;
        case Game::SelectionContext::FEAST_GAIN:
            if (!selected.empty()) {
                player->addCardToDiscardStack(game->getCardFromCardDistrib(selected.front()->getName()));
            }
            break;
        case Game::SelectionContext::THRONE_ROOM_PLAY:
            if (!selected.empty()) {
                const Action *action = dynamic_cast<const Action *>(selected.front());
                if (action) {
                    action->play(*game, *player);
                    if (!game->hasPendingSelection()) {
                        action->play(*game, *player);
                    }
                }
            }
            break;
        case Game::SelectionContext::MILITIA_DISCARD:
            for (const Card *card : selected) player->discardCard(card);
            if (!game->getPendingPlayers().empty()) {
                const std::set<Player*> remaining(game->getPendingPlayers().begin(),
                                                   game->getPendingPlayers().end());
                game->clearPendingPlayers();
                const Attack *attack = dynamic_cast<const Attack *>(game->getPendingCard());
                if (attack) attack->attack(*game, *game->getPendingActor(), remaining);
            }
            break;
        case Game::SelectionContext::BUREAUCRAT_TOPDECK:
            if (!selected.empty()) {
                player->removeCardFromHand(selected.front());
                player->addCardToDrawStack(selected.front());
            }
            if (!game->getPendingPlayers().empty()) {
                const std::set<Player*> remaining(game->getPendingPlayers().begin(),
                                                   game->getPendingPlayers().end());
                game->clearPendingPlayers();
                const Attack *attack = dynamic_cast<const Attack *>(game->getPendingCard());
                if (attack) attack->attack(*game, *game->getPendingActor(), remaining);
            }
            break;
        case Game::SelectionContext::THIEF_TRASH: {
            std::vector<const Card*> kept;
            for (const Card *card : game->getPendingThiefTreasures()) {
                if (std::find(selected.begin(), selected.end(), card) != selected.end()) kept.push_back(card);
                else game->trashCard(card);
            }
            game->setPendingThiefTreasures(kept);
            if (!game->getPendingPlayers().empty()) {
                const std::set<Player*> remaining(game->getPendingPlayers().begin(),
                                                   game->getPendingPlayers().end());
                game->clearPendingPlayers();
                const Attack *attack = dynamic_cast<const Attack *>(game->getPendingCard());
                if (attack) attack->attack(*game, *game->getPendingActor(), remaining);
            } else if (!kept.empty()) {
                game->requestSelection(game->getPendingActor(), kept, kept.size(),
                                       SelectionMode::GAIN, SelectionConstraint::AT_MOST,
                                       Game::SelectionContext::THIEF_GAIN);
            }
            break;
        }
        case Game::SelectionContext::THIEF_GAIN:
            for (const Card *card : game->getPendingThiefTreasures()) {
                if (std::find(selected.begin(), selected.end(), card) != selected.end()) {
                    player->addCardToDiscardStack(card);
                } else {
                    game->trashCard(card);
                }
            }
            game->clearPendingThiefTreasures();
            break;
        case Game::SelectionContext::SPY_CARD: {
            Player *target = game->getPendingSelectionTarget();
            if (target && !selected.empty()) target->addCardToDiscardStack(selected.front());
            else if (target && !offered.empty()) target->addCardToDrawStack(offered.front());
            if (!game->getPendingPlayers().empty()) {
                const std::set<Player*> remaining(game->getPendingPlayers().begin(),
                                                   game->getPendingPlayers().end());
                game->clearPendingPlayers();
                const Attack *attack = dynamic_cast<const Attack*>(game->getPendingCard());
                if (attack) attack->attack(*game, *game->getPendingActor(), remaining);
            }
            game->setPendingSelectionTarget(nullptr);
            break;
        }
        case Game::SelectionContext::LIBRARY_ACTION: {
            const Card *card = offered.front();
            if (selected.empty()) player->addCardToHand(card);
            else game->getPendingLibrarySkipped().push_back(card);
            const Action *library = dynamic_cast<const Action*>(game->getPendingCard());
            if (library) library->play(*game, *player);
            break;
        }
        case Game::SelectionContext::CHANCELLOR_DISCARD:
            if (!selected.empty()) player->discardDrawStack();
            break;
        case Game::SelectionContext::MONEYLENDER_TRASH:
            if (!selected.empty()) {
                player->removeCardFromHand(selected.front());
                game->trashCard(selected.front());
                game->addGold(3);
            }
            break;
        default:
            sendAction(
                QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                Network::ErrorPayload("This selection is not implemented yet.").toJson(),
                client->getSocket());
            return;
    }

    sendGameStateToRoom(room);
    sendSelectionRequest(room, game);
}

void GameServer::handlePlayAllTreasures(Client *client) {
    int oldGold;
    int newGold;
    if (Room *room = RoomManager::getInstance().getRoomByClient(client)) {
        oldGold = room->getGame()->getCurrentPlayerMoney();
        if (Game *game = room->getGame()) {
            if(!game->tryPlayAllTreasures(client->getAssociatedPlayer())){

                const std::string message = "PLAY ALL TREASURES ERROR FROM PLAYER " + client->getAssociatedPlayer()->getName();
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                    Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                    client->getSocket()
                );
                return;
            }
            newGold = room->getGame()->getCurrentPlayerMoney();
            for (Client* cl : room->getClients()) {
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::GAINED_MONEY)),
                    Network::GainedMoneyPayload(newGold - oldGold).toJson(),
                    cl->getSocket()
                );
            }
            sendGameStateToRoom(room);
        }
    }
}

void GameServer::handleBuyCard(const QJsonObject &data, Client *client) {
    if (Room *room = RoomManager::getInstance().getRoomByClient(client)) {
        if (Game *game = room->getGame()) {
            auto payload = Network::BuyCardPayload::fromJson(data);
            if (!payload) {
                qWarning() << "Invalid data given" << data;
                return;
            }

            const Card* cardToBuy = payload->cardToBuy;
            if (!cardToBuy) {
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                    Network::ErrorPayload("Unknown card.").toJson(),
                    client->getSocket()
                );
                return;
            }

            if(!game->tryBuyCard(client->getAssociatedPlayer(), cardToBuy)){
                const std::string message = "BUY CARD ERROR : " + cardToBuy->getName() + " FROM PLAYER " + client->getAssociatedPlayer()->getName();
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                    Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                    client->getSocket()
                );
                return;
            }

            for(Client* cl : room->getClients()) {
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::BUY_CARD)),
                    Network::CardBought(cardToBuy).toJson(),
                    cl->getSocket()
                );
            }
            sendGameStateToRoom(room);
        }
    }
}

void GameServer::handleReaction(const QJsonObject &data, Client* client){
    if (Room *room = RoomManager::getInstance().getRoomByClient(client)) {
        if (Game *game = room->getGame()) {
            auto payload = Network::ReactionResponsePayload::fromJson(data);
            if (!payload || !game->isPlayerPending(client->getAssociatedPlayer())) {
                qWarning() << "Invalid data given" << data;
                return;
            }
            const EventType event = client->getAssociatedPlayer() == game->getPendingActor()
                ? EventType::ATTACK_PLAYED_SELF
                : EventType::ATTACK_PLAYED_OTHER;

            if (payload->isReacting) {
                if (!payload->reactionCard ||
                    !game->tryPlayReaction(client->getAssociatedPlayer(),
                                           payload->reactionCard,
                                           event,
                                           game->getPendingActor())) {
                    const std::string message = "REACTION CARD ERROR FROM PLAYER " +
                        client->getAssociatedPlayer()->getName();
                    sendAction(
                        QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                        Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                        client->getSocket()
                    );
                    return;
                }
            } else {
                game->removePendingPlayers(client->getAssociatedPlayer());
            }

            for(Client* cl : room->getClients()) {
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::REACTION)),
                    Network::ReactionResponsePayload(
                        payload->playerName,
                        payload->isReacting,
                        payload->reactionCard
                    ).toJson(),
                    cl->getSocket()
                );
            }

            if(!game->waitingForPlayers()) {
                const Card* card = game->getPendingCard();
                Player* actor = game->getPendingActor();
                game->setPendingCard(nullptr);
                game->setPendingActor(nullptr);
                game->setPendingEvent(EventType::INVALID_EVENT);
                game->clearPendingPlayers();
                switch(game->getCurrentType()) {
                    case CardType::ATTACK:
                        if(!card || !actor || !game->tryPlayAttack(actor, card)){
                            const std::string message = "ATTACK CARD ERROR FROM PLAYER " + client->getAssociatedPlayer()->getName();
                            sendAction(
                                QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                                Network::ErrorPayload(QString::fromStdString(message)).toJson(),
                                client->getSocket()
                            );
                            return;
                        }

                        for(Client* cl : room->getClients()){
                            sendAction(
                                QString::fromStdString(actionTypeToString(ActionType::PLAY_CARD)),
                                Network::AttackPlayed(
                                    game->getCurrentTargets(),
                                    card
                                ).toJson(),
                                cl->getSocket()
                            );
                        }
                        sendGameStateToRoom(room);
                        game->setCurrentTargets({});
                        sendSelectionRequest(room, game);
                        return;

                    default:
                        return;
                }
            }

        }
    }

}

void GameServer::handleEndTurn(Client *client) {
    if (Room *room = RoomManager::getInstance().getRoomByClient(client)) {
        if (Game *game = room->getGame()) {
            if (!game->isPlayerTurn(client->getAssociatedPlayer())) {
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                    Network::ErrorPayload("It's not your turn.").toJson(),
                    client->getSocket());
                return;
            }
            if (game->getCurrentPhase() != GamePhase::BUY) {
                sendAction(
                    QString::fromStdString(actionTypeToString(ActionType::ERROR)),
                    Network::ErrorPayload("The buy phase has not ended.").toJson(),
                    client->getSocket());
                return;
            }
            if(game->performCleanup()){
                Player winner = game->getWinner();
                for(Client* cl : room->getClients()) {
                    sendAction(
                        QString::fromStdString(actionTypeToString(ActionType::GAME_ENDED)),
                        Network::GameEndedPayload(winner.getName().c_str()).toJson(),
                        cl->getSocket()
                    );
                }
            } else{
                for(Client* cl : room->getClients()) {
                    sendAction(
                        QString::fromStdString(actionTypeToString(ActionType::END_TURN)),
                        Network::TurnEndedPayload(game->getActivePlayer().getName().c_str()).toJson(),
                        cl->getSocket()
                    );
                    sendAction(
                        QString::fromStdString(actionTypeToString(ActionType::GAME_STATE)),
                        room->getGameStatePayload(cl).toJson(),
                        cl->getSocket()
                    );
                }
            }
        }
    }
}

void GameServer::sendAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket){

    std::cout << "Sending action: " << action.toStdString() << " to client ID: "<< m_clients[clientSocket]->getId() << std::endl;

    QJsonObject message;
    message["action"] = action;
    message["data"] = data;

    QJsonDocument doc(message);
    QByteArray toSend = doc.toJson(QJsonDocument::Compact) + '\n';
    std::cout << "Sending JSON: " << toSend.toStdString() << std::endl;
    clientSocket->write(toSend);
}
