#include <QApplication>
#include "client/MainWindow.hpp"
#include "client/network/NetworkManager.hpp"
#include "client/ClientPreferences.hpp"


int main(int argc, char *argv[]) {

  QApplication app(argc, argv);
  app.setStyleSheet("QWidget { background-color: #1e1e1e; color: #ffffff; }");
  ClientPreferences clientPreferences;
  NetworkManager networkManager(clientPreferences);
  MainWindow mainWindow(nullptr, &networkManager, &clientPreferences);

  mainWindow.show();
  networkManager.connectToServer();

  return app.exec();
}