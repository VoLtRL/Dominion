#pragma once

#include <QObject>
#include <QtTypes>
#include <QHostAddress>
#include <QTcpSocket>
#include <QJsonObject>
#include "shared/structs/ActionType.hpp"
#include "shared/structs/Payloads.hpp"

class NetworkManager : public QObject {
    Q_OBJECT

private:
    QTcpSocket m_socket;
    QHostAddress m_host;
    quint16 m_port;

public:
    explicit NetworkManager(QObject *parent = nullptr);
    QHostAddress getHost() const;
    quint16 getPort() const;
    void connectToServer(const QHostAddress &host, quint16 port);
    void disconnectFromServer();
    void sendAction(const ActionType &action, const Network::Payload &data);
    QAbstractSocket::SocketState getState() const;
    
signals:
    void connected();
    void disconnected();
    void actionReceived(const ActionType &action, const QJsonObject &data);
    void connectionError(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);


};