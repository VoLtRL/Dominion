#include <QApplication>
#include <QHostAddress>

#include "client/MainWindow.hpp"
#include "client/network/NetworkManager.hpp"


int main(int argc, char *argv[]) {

  QApplication app(argc, argv);
  NetworkManager networkManager; // Create an instance of NetworkManager
  networkManager.connectToServer(QHostAddress("127.0.0.1"), 4040); // Connect to the server at localhost:4040
  MainWindow mainWindow(nullptr, &networkManager);

  mainWindow.show();

  return app.exec();
}