#include <QApplication>
#include "client/MainWindow.hpp"
#include "client/network/NetworkManager.hpp"
#include "client/ClientPreferences.hpp"


int main(int argc, char *argv[]) {

  QApplication app(argc, argv);
  ClientPreferences clientPreferences;
  NetworkManager networkManager(clientPreferences);
  MainWindow mainWindow(nullptr, &networkManager, &clientPreferences);

  mainWindow.show();
  networkManager.connectToServer();

  return app.exec();
}