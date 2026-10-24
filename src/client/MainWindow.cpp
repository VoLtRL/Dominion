#include "client/MainWindow.hpp"
#include "client/MainMenu.hpp"
#include "client/CreateGame.hpp"
#include "client/GameLobby.hpp"
#include "client/JoinRoom.hpp"
#include "client/GameTable.hpp"
#include "shared/structs/Payloads.hpp"
#include "shared/structs/ActionType.hpp"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent, NetworkManager *networkManager) : QMainWindow(parent), ui(new Ui::MainWindow), networkManager(networkManager) {
    ui->setupUi(this);

    playerName = "Player_" + QString::number(QRandomGenerator::global()->bounded(10000));

    // Create the main menu
    MainMenu *mainMenu = new MainMenu(this);
    CreateGame *createGame = new CreateGame(this);
    GameLobby *gameLobby = new GameLobby(this);
    GameTable *gameTable = new GameTable(this);
    JoinRoom *joinRoom = new JoinRoom(this);

    int menuIndex = ui->stackedWidget->addWidget(mainMenu);
    int createGameIndex = ui->stackedWidget->addWidget(createGame);
    int gameLobbyIndex = ui->stackedWidget->addWidget(gameLobby);
    int gameTableIndex = ui->stackedWidget->addWidget(gameTable);
    int joinRoomIndex = ui->stackedWidget->addWidget(joinRoom);
    ui->stackedWidget->setCurrentIndex(menuIndex);

    connect(mainMenu, &MainMenu::createGameClicked, this, [this, createGameIndex, networkManager]() {
        if (networkManager->getState() != QAbstractSocket::ConnectedState) {
            ui->statusbar->showMessage("Not connected to server. Trying to reconnect...");
            networkManager->connectToServer(networkManager->getHost(), networkManager->getPort());
            return;
        }
        ui->stackedWidget->setCurrentIndex(createGameIndex);
    });

    connect(mainMenu, &MainMenu::joinRoomClicked, this, [this, joinRoomIndex, networkManager]() {
        if (networkManager->getState() != QAbstractSocket::ConnectedState) {
            ui->statusbar->showMessage("Not connected to server. Trying to reconnect...");
            networkManager->connectToServer(networkManager->getHost(), networkManager->getPort());
            return;
        }
        ui->stackedWidget->setCurrentIndex(joinRoomIndex);
        networkManager->sendAction(ActionType::REQUEST_ROOM_LIST, Network::EmptyPayload());
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

        Network::CreateRoomPayload roomData = createGame->getRoomData();
        roomData.playerName = playerName;



        networkManager->sendAction(ActionType::CREATE_ROOM, roomData);
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

    connect(joinRoom, &JoinRoom::goBackToMenuClicked, this, [this, menuIndex]() {
        ui->stackedWidget->setCurrentIndex(menuIndex);
    });

    connect(joinRoom, &JoinRoom::joinRoomClicked, this, [this, networkManager, gameLobbyIndex](int roomId) {
        if (networkManager->getState() != QAbstractSocket::ConnectedState) {
            ui->statusbar->showMessage("Not connected to server. Please check your connection.");
            return;
        }

        Network::JoinRoomPayload join_payload = Network::JoinRoomPayload(roomId, playerName);

        networkManager->sendAction(ActionType::JOIN_ROOM, join_payload);
    });



    connect(networkManager, &NetworkManager::actionReceived, this, [this,gameLobby, gameLobbyIndex, gameTableIndex](const ActionType &action, const QJsonObject &data) {
        if (action == ActionType::ROOM_CREATED) {
            ui->stackedWidget->setCurrentIndex(gameLobbyIndex);

            auto payload = Network::CreatedRoomPayload::fromJson(data);
            if(!payload.has_value()){
                qWarning() << "Invalid data received for ROOM_CREATED action: " << data;
                return;
            }
            
            gameLobby->clearPlayerList();
            gameLobby->updateRoomInfo(payload.value());
            gameLobby->addPlayerToList(playerName);

        } else if (action == ActionType::ROOM_CREATION_FAILED) {
            QString errorMessage = data.value("error").toString();
            ui->statusbar->showMessage("Room creation failed: " + errorMessage);
        } else if (action == ActionType::ROOM_JOIN_FAILED) {
            ui->statusbar->showMessage("Room join failed: " + data.value("error").toString());
        } else if (action == ActionType::PLAYER_JOINED) {
            auto payload = Network::PlayerJoinedRoomPayload::fromJson(data);
            if(!payload.has_value()){
                qWarning() << "Invalid data received for PLAYER_JOINED action: " << data;
                return;
            }
            gameLobby->addPlayerToList(payload->playerName);
        }
        else if (action == ActionType::JOIN_ROOM) {
            auto payload = Network::RoomJoinedPayload::fromJson(data);
            if(!payload.has_value()){
                qWarning() << "Invalid data received for JOIN_ROOM action: " << data;
                return;
            }
            ui->stackedWidget->setCurrentIndex(gameLobbyIndex);
            gameLobby->clearPlayerList();
            gameLobby->updateRoomInfo(payload.value());

        }
         else if (action == ActionType::PLAYER_LEFT) {
            auto payload = Network::PlayerLeftRoomPayload::fromJson(data);
            if(!payload.has_value()){
                qWarning() << "Invalid data received for PLAYER_LEFT action: " << data;
                return;
            }
            gameLobby->removePlayerFromList(payload->playerName);
        } else if (action == ActionType::START_GAME) {
            auto payload = Network::StartGamePayload::fromJson(data);
            if(!payload.has_value()){
                qWarning() << "Invalid data received for START_GAME action: " << data;
                return;
            }
            ui->stackedWidget->setCurrentIndex(gameTableIndex);
        } else if (action == ActionType::REQUEST_ROOM_LIST) {
            auto payload = Network::RoomListPayload::fromJson(data);
            if(!payload.has_value()){
                qWarning() << "Invalid data received for REQUEST_ROOM_LIST action: " << data;
                return;
            }
            JoinRoom *joinRoom = qobject_cast<JoinRoom*>(ui->stackedWidget->widget(ui->stackedWidget->currentIndex()));
            if(joinRoom){
                joinRoom->updateAvailableRooms(payload.value().rooms);
            }
        }
    });

}


MainWindow::~MainWindow() {
    delete ui;
}