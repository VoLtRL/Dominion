#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    qDebug() << "Dominion Server initialisé sur le port 4242...";

    return app.exec();
}