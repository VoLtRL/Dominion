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
    Client *client = m_clients[clientSocket];
    if (!client) return;

    auto &roomMgr = RoomManager::getInstance();

    std::cout << "Received action: " << action.toStdString() << " from client ID: "<< client->getId() << std::endl;

    switch (stringToActionType(action.toStdString())) {
        case ActionType::CREATE_ROOM: {
            auto payload_create = Network::CreateRoomPayload::fromJson(data);
            if(!payload_create){
                qWarning() << "Invalid data given" << data;
                return;
            }
            client->setName(payload_create->playerName);
            roomMgr.createRoom(*payload_create, client);

            Network::CreatedRoomPayload to_send_created = Network::CreatedRoomPayload(roomMgr.getRoomByClient(client)->getId(), payload_create->roomName);
            sendAction(QString::fromStdString(actionTypeToString(ActionType::ROOM_CREATED)), to_send_created.toJson(), client->getSocket());
            break;
        }
        case ActionType::JOIN_ROOM: {
            auto payload_join = Network::JoinRoomPayload::fromJson(data);
            if(!payload_join.has_value()){
                qWarning() << "Invalid data given" << data;
                return;
            }

            client->setName(payload_join->playerName);
            Room* targetRoom = roomMgr.getRoomById(payload_join->roomId);
            if (!targetRoom || !roomMgr.joinRoom(client, targetRoom)) {
                sendAction(QString::fromStdString(actionTypeToString(ActionType::ROOM_JOIN_FAILED)),
                           QJsonObject{{"error", "Room is unavailable."}}, clientSocket);
                break;
            }

            std::vector<QString> playerNames;
            for (const Client *roomClient : targetRoom->getClients()) {
                playerNames.push_back(roomClient->getName());
            }
            Network::RoomInfo roomInfo(targetRoom->getId(), targetRoom->getName(),
                                       playerNames, targetRoom->getPlacesLeft());
            Network::RoomJoinedPayload roomJoined(roomInfo, playerNames);
            sendAction(QString::fromStdString(actionTypeToString(ActionType::JOIN_ROOM)),
                       roomJoined.toJson(), clientSocket);

            Network::PlayerJoinedRoomPayload to_send_joined = Network::PlayerJoinedRoomPayload(client->getName());

            for(Client* c : targetRoom->getClients()){
                if (c != client) {
                    sendAction(QString::fromStdString(actionTypeToString(ActionType::PLAYER_JOINED)),to_send_joined.toJson(), c->getSocket());
                }
            };

            break;
        }
        case ActionType::REQUEST_ROOM_LIST: {
            Network::RoomListPayload roomListJson = Network::RoomListPayload({});
            for (const Room* room : roomMgr.getAvailableRooms()) {
                std::vector<QString> playerNames;
                for (const Client *roomClient : room->getClients()) {
                    playerNames.push_back(roomClient->getName());
                }
                roomListJson.rooms.push_back(Network::RoomInfo(
                    room->getId(), room->getName(), playerNames, room->getPlacesLeft()));
            }
            sendAction(QString::fromStdString(actionTypeToString(ActionType::REQUEST_ROOM_LIST)), roomListJson.toJson(), clientSocket);
            break;
        }
        case ActionType::LEAVE_ROOM:
            roomMgr.leaveRoom(client);
            break;
        case ActionType::START_GAME:
            roomMgr.startGame(client);
            break;
        case ActionType::PLAY_CARD: {
            if (Room *room = roomMgr.getRoomByClient(client)) {
                if (Game *game = room->getGame()) {
                }
            }
            break;
        }
        case ActionType::END_TURN: {
            if (Room *room = roomMgr.getRoomByClient(client)) {
                if (Game *game = room->getGame()) {
                }
            }
            break;
        }
        default:
            qWarning() << "Received invalid action:" << action;
            break;
    }
}

void GameServer::sendAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket){

    std::cout << "Sending action: " << action.toStdString() << " to client ID: "<< m_clients[clientSocket]->getId() << std::endl;

    QJsonObject message;
    message["action"] = action;
    message["data"] = data;

    QJsonDocument doc(message);
    QByteArray toSend = doc.toJson(QJsonDocument::Compact) + '\n';
    clientSocket->write(toSend);
}
