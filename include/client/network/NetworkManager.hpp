#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

class NetworkManager : public QObject {
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    void connectToServer(const QHostAddress &host, quint16 port);
    void disconnectFromServer();
    void sendAction(const QString &action, const QJsonObject &data);
    QAbstractSocket::SocketState getState() const;

signals:
    void connected();
    void disconnected();
    void actionReceived(const QString &action, const QJsonObject &data);

    void connectionError(const QString &errorString);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket m_socket;
};