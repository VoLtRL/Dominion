#include "server/GameServer.hpp"
#include "server/Client.hpp"
#include "server/RoomManager.hpp"
#include "shared/structs/ActionType.hpp"
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
            handlePlayCard(client);
            break;
        case ActionType::END_TURN:
            handleEndTurn(client);
            break;
        default:
            qWarning() << "Received invalid action:" << action;
            break;
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

    targetRoom->addPlayer(client);

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
    if (!room /*|| !roomMgr.startGame(client)*/) {
        return;
    }

    Network::StartGamePayload gameStarted(room->getName());
    for (Client *roomClient : room->getClients()) {
        sendAction(
            QString::fromStdString(actionTypeToString(ActionType::START_GAME)),
            gameStarted.toJson(),
            roomClient->getSocket());
    }
}

void GameServer::handlePlayCard(Client *client) {
    if (Room *room = RoomManager::getInstance().getRoomByClient(client)) {
        if (Game *game = room->getGame()) {
        }
    }
}

void GameServer::handleEndTurn(Client *client) {
    if (Room *room = RoomManager::getInstance().getRoomByClient(client)) {
        if (Game *game = room->getGame()) {
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
