#include "client/network/NetworkManager.hpp"
#include <iostream>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent), m_socket(new QTcpSocket(this)) {
    connect(&m_socket, &QTcpSocket::connected, this, &NetworkManager::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &NetworkManager::onDisconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onReadyRead);
    connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkManager::onErrorOccurred);
}

QAbstractSocket::SocketState NetworkManager::getState() const {
    return m_socket.state();
}

void NetworkManager::onConnected() {
    emit connected();
}

void NetworkManager::onDisconnected() {
    emit disconnected();
}

void NetworkManager::onReadyRead() {
    while (m_socket.canReadLine()) {
    QByteArray line = m_socket.readLine();
    QJsonDocument doc = QJsonDocument::fromJson(line);
    if (!doc.isObject()) {
        emit connectionError("Received invalid JSON from server.");
        continue;
    
    }
    QJsonObject data = doc.object();
    emit actionReceived(data["action"].toString(), data["data"].toObject());
    }
}

void NetworkManager::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    emit connectionError(m_socket.errorString());
}

void NetworkManager::connectToServer(const QHostAddress &host, quint16 port) {
    m_socket.connectToHost(host, port);
    if (m_socket.waitForConnected(3000)) {
    qDebug() << "Connected to local server successfully!";
} else {
    qWarning() << "Could not connect:" << m_socket.errorString();
}
}

void NetworkManager::disconnectFromServer() {
    m_socket.disconnectFromHost();
}

void NetworkManager::sendAction(const QString &action, const QJsonObject &data) {
    if (m_socket.state() != QAbstractSocket::ConnectedState) {
        emit connectionError("Not connected to server.");
        return;
    }
    QJsonObject message;
    message["action"] = action;
    message["data"] = data;

    QJsonDocument doc(message);
    QByteArray toSend = doc.toJson(QJsonDocument::Compact) + '\n';
    m_socket.write(toSend);

}