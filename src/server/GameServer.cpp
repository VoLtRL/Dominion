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
}

void GameServer::receiveAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket) {
    Client *client = m_clients[clientSocket];
    if (!client) return;

    auto &roomMgr = RoomManager::getInstance();

    switch (stringToActionType(action.toStdString())) {
        case ActionType::CREATE_ROOM:
            auto payload_create = Network::CreateRoomPayload::fromJson(data);
            if(!payload_create){
                qWarning() << "Invalid data given" << data;
                return;
            }
            roomMgr.createRoom(*payload_create, client);
            break;
        case ActionType::JOIN_ROOM:
            auto payload_join = Network::JoinRoomPayload::fromJson(data);
            if(!payload_join){
                qWarning() << "Invalid data given" << data;
                return;
            }

            Room* targetRoom = roomMgr.getRoomById(payload_join->roomId());
            roomMgr.joinRoom(client, targetRoom);

            Network::PlayerJoinedRoomPayload toSend = Network::PlayerJoinedRoomPayload(client->getName());

            for(Client* c : targetRoom->getClients()){
                sendAction(QString::fromStdString(actionTypeToString(ActionType::PLAYER_JOINED)),toSend.toJson(), c->getSocket());
            };

            break;
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
    QJsonObject message;
    message["action"] = action;
    message["data"] = data;

    QJsonDocument doc(message);
    QByteArray toSend = doc.toJson(QJsonDocument::Compact) + '\n';
    clientSocket->write(toSend);
}

