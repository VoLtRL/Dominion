#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include "server/GameServer.hpp"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    GameServer server;
    server.startServer(4040);

    return app.exec();
}