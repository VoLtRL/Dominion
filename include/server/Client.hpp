#pragma once

#include <QString>

class Client {
    private:
        Qint64 m_id;
        QString m_name;
        QString m_roomName;
        bool m_isBot;

public:
    Client(const QString &name, const QString &roomName, bool isBot = false);
};