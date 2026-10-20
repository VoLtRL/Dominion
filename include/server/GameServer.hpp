#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

#include "server/Client.hpp"

class GameServer : public QObject {
    Q_OBJECT
private:
    QTcpServer m_server;
    QMap<QTcpSocket*, const Client*> m_clients;

public:
    explicit GameServer(QObject *parent = nullptr);
    void startServer(quint16 port);
    void stopServer();
    void receiveAction(const QString &action, const QJsonObject &data, QTcpSocket *clientSocket);

};