#pragma once

#include <QString>
#include <QtGlobal>
#include <optional>

class ClientPreferences {

private:
    QString playerName{"Player"};
    QString hostAddress{"localhost"};
    quint16 hostPort{4040};
    std::optional<int> currentRoomId{std::nullopt};


public:
    ClientPreferences() = default;

    const QString &getPlayerName() const;
    void setPlayerName(const QString &name);

    const QString &getHostAddress() const;
    void setHostAddress(const QString &address);

    quint16 getHostPort() const;
    void setHostPort(quint16 port);

    std::optional<int> getCurrentRoomId() const;
    void setCurrentRoomId(std::optional<int> roomId);
};
