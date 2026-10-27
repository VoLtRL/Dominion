#include "client/ClientPreferences.hpp"

const QString &ClientPreferences::getPlayerName() const {
    return playerName;
}

void ClientPreferences::setPlayerName(const QString &name) {
    const QString trimmedName = name.trimmed();
    if (!trimmedName.isEmpty()) {
        playerName = trimmedName;
    }
}

const QString &ClientPreferences::getHostAddress() const {
    return hostAddress;
}

void ClientPreferences::setHostAddress(const QString &address) {
    const QString trimmedAddress = address.trimmed();
    if (!trimmedAddress.isEmpty()) {
        hostAddress = trimmedAddress;
    }
}

quint16 ClientPreferences::getHostPort() const {
    return hostPort;
}

void ClientPreferences::setHostPort(quint16 port) {
    if (port != 0) {
        hostPort = port;
    }
}

std::optional<int> ClientPreferences::getCurrentRoomId() const {
    return currentRoomId;
}

void ClientPreferences::setCurrentRoomId(std::optional<int> roomId) {
    currentRoomId = roomId;
}