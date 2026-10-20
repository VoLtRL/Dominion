#include "client/MainWindow.hpp"
#include "client/MainMenu.hpp"
#include "client/CreateGame.hpp"
#include "client/GameLobby.hpp"
#include "client/GameTable.hpp"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent, NetworkManager *networkManager) : QMainWindow(parent), ui(new Ui::MainWindow), networkManager(networkManager) {
    ui->setupUi(this);

    // Create the main menu
    MainMenu *mainMenu = new MainMenu(this);
    CreateGame *createGame = new CreateGame(this);
    GameLobby *gameLobby = new GameLobby(this);
    GameTable *gameTable = new GameTable(this);

    int menuIndex = ui->stackedWidget->addWidget(mainMenu);
    int createGameIndex = ui->stackedWidget->addWidget(createGame);
    int gameLobbyIndex = ui->stackedWidget->addWidget(gameLobby);
    int gameTableIndex = ui->stackedWidget->addWidget(gameTable);

    ui->stackedWidget->setCurrentIndex(menuIndex);

    connect(mainMenu, &MainMenu::createGameClicked, this, [this, createGameIndex, networkManager]() {
        if (networkManager->getState() != QAbstractSocket::ConnectedState) {
            ui->statusbar->showMessage("Not connected to server. Please check your connection.");
            return;
        }
        ui->stackedWidget->setCurrentIndex(createGameIndex);
    });

    connect(mainMenu, &MainMenu::joinGameClicked, this, [this, networkManager]() {
        if (networkManager->getState() != QAbstractSocket::ConnectedState) {
            ui->statusbar->showMessage("Not connected to server. Please check your connection.");
            return;
        }
        // Handle join game action
    });

    connect(mainMenu, &MainMenu::leaderboardClicked, this, [this]() {
        // Handle leaderboard action
    });

    connect(mainMenu, &MainMenu::quitClicked, this, [this]() {
        close();
    });

    connect(createGame, &CreateGame::goBackToMenuClicked, this, [this, menuIndex]() {
        ui->stackedWidget->setCurrentIndex(menuIndex);
    });

    connect(createGame, &CreateGame::createRoomClicked, this, [this, gameLobbyIndex, createGame, networkManager]() {
        if (networkManager->getState() != QAbstractSocket::ConnectedState) {
            ui->statusbar->showMessage("Not connected to server. Please check your connection.");
            return;
        }
        networkManager->sendAction("CREATE_ROOM", createGame->getRoomData());
        ui->stackedWidget->setCurrentIndex(gameLobbyIndex);
    });

    connect(gameLobby, &GameLobby::goBackToMenuClicked, this, [this, menuIndex]() {
        ui->stackedWidget->setCurrentIndex(menuIndex);
    });

    connect(gameLobby, &GameLobby::launchGameClicked, this, [this, gameTableIndex]() {
        ui->stackedWidget->setCurrentIndex(gameTableIndex);
    });

    connect(networkManager, &NetworkManager::connected, this, [this]() {
        ui->statusbar->showMessage("Connected to server");
    });

    connect(networkManager, &NetworkManager::disconnected, this, [this, networkManager]() {
        ui->statusbar->showMessage("Disconnected from server. Trying to reconnect...");
        while (networkManager->getState() != QAbstractSocket::ConnectedState) {
            networkManager->connectToServer(QHostAddress("127.0.0.1"), 4040);
            QThread::sleep(1); // Wait for 1 second before trying again
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}