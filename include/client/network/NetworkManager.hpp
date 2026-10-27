#pragma once

#include <QObject>
#include <QtTypes>
#include <QTcpSocket>
#include <QString>
#include <QJsonObject>
#include "shared/structs/ActionType.hpp"
#include "shared/structs/Payloads.hpp"
#include "client/ClientPreferences.hpp"

class NetworkManager : public QObject {
    Q_OBJECT

private:
    QTcpSocket m_socket;
    const ClientPreferences &clientPreferences;

public:
    explicit NetworkManager(const ClientPreferences &clientPreferences, QObject *parent = nullptr);
    void connectToServer();
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