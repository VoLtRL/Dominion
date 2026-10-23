#include "server/Client.hpp"

qint64 Client::generateId() {
    static qint64 nextId = 1;
    return nextId++;
}

void Client::sendJson(const QString &actionType, const QJsonObject &payload) {
    if (!m_socket || m_isBot || m_socket->state() != QAbstractSocket::ConnectedState) {
        return;
    }

    QJsonObject message = payload;
    message["action"] = actionType;
    
    QByteArray data = QJsonDocument(message).toJson(QJsonDocument::Compact) + '\n';
    m_socket->write(data);
}