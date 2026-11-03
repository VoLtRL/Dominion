#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonObject>
#include <unordered_map>
#include <chrono>
#include <thread>

#include "server/Client.hpp"
#include "shared/structs/Payloads.hpp"
#include "server/RoomManager.hpp"
#include "server/ai/BotManager.hpp"

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
    void handlePlayAllTreasures(Client *client);
    void handleBuyCard(const QJsonObject &data, Client *client);
    void handleNextPhase(Client *client);
    void handleEndTurn(Client *client);
    void handleSelection(const QJsonObject &data, Client *client);
    void handleReaction(const QJsonObject &data, Client *client);
    void sendSelectionRequest(Room *room, Game *game);
    void sendSelectionMade(Room *room, Player *player,
                           const std::vector<const Card*> &cards);

public:
    explicit GameServer(QObject *parent = nullptr);
    ~GameServer() = default;
    void startServer(quint16 port = 4040);
    void stopServer();

    void sendGameStateToRoom(Room *room);

    void receiveAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket);

    void sendAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket);

    Client* getClientById(int id);

    void playBotTurn(Game* game, Bot* bot, Room* room);

    bool botActionPhase(Game* game, Bot* bot, Room* room); // True if waiting for reactions
    void botBuyPhase(Game* game, Bot* bot, Room* room);

};