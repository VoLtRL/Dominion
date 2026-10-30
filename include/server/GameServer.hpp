#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonObject>
#include <unordered_map>

#include "server/Client.hpp"
#include "shared/structs/Payloads.hpp"

class GameServer : public QObject {
    Q_OBJECT
private:
    QTcpServer m_server;
    std::unordered_map<QTcpSocket*, Client*> m_clients;

    void handleCreateRoom(const QJsonObject &data, Client *client);
    void handleJoinRoom(const QJsonObject &data, Client *client);
    void handleRequestRoomList(Client *client);
    void handleLeaveRoom(Client *client);
    void handleStartGame(Client *client);
    void handlePlayCard(const QJsonObject &data, Client *client);
    void handleNextPhase(Client *client);
    void handleEndTurn(Client *client);
    void handleSelection(const QJsonObject &data, Client *client);
    void handleReaction(const QJsonObject &data, Client *client);

public:
    explicit GameServer(QObject *parent = nullptr);
    ~GameServer() = default;
    void startServer(quint16 port = 4040);
    void stopServer();
    void receiveAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket);

    void sendAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket);

    Client* getClientById(int id);

};