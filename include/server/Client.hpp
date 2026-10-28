#pragma once

#include <QString>
#include <QtTypes>
#include <memory>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

#include "shared/player/Player.hpp"

class Client {
private:
    qint64 m_id;
    QString m_name;
    QString m_roomName;
    bool m_isBot;
    
    QTcpSocket* m_socket; 

    Player* m_associatedPlayer = nullptr;

    static qint64 generateId();

public:
    Client(QTcpSocket* socket, const QString &name = "Invité", bool isBot = false)
        : m_socket(socket), m_id(generateId()), m_name(name), m_isBot(isBot) {}

    ~Client() = default;

    qint64 getId() const { return m_id; }
    QString getName() const { return m_name; }
    QString getRoomName() const { return m_roomName; }
    bool isBot() const { return m_isBot; }
    QTcpSocket* getSocket() const { return m_socket; }

    void setName(const QString &name) { m_name = name; }
    void setRoomName(const QString &roomName) { m_roomName = roomName; }

    void setAssociatedPlayer(Player* player) { m_associatedPlayer = player; }
    Player* getAssociatedPlayer() const { return m_associatedPlayer; }

    void sendJson(const QString &actionType, const QJsonObject &payload = {});
};