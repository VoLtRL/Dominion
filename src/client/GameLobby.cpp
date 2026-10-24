#include "client/GameLobby.hpp"

#include "ui_GameLobby.h"

GameLobby::GameLobby(QWidget *parent) : QWidget(parent), ui(new Ui::GameLobby) {
    ui->setupUi(this);

    connect(ui->goBackToMenuButton, &QPushButton::clicked, this, &GameLobby::onGoBackToMenuClicked);
    connect(ui->launchGameButton, &QPushButton::clicked, this, &GameLobby::onLaunchGameClicked);
}

void GameLobby::onGoBackToMenuClicked() {
    emit goBackToMenuClicked();
}

void GameLobby::onLaunchGameClicked() {
    emit launchGameClicked();
}

void GameLobby::updateRoomInfo(const Network::CreatedRoomPayload &data) {
    roomId = data.roomId;
    QString roomName = data.roomName;

    ui->room_name->setText("Room Name: " + roomName);
}

void GameLobby::updateRoomInfo(const Network::RoomJoinedPayload &data) {
    roomId = data.roomInfo.roomId;
    QString roomName = data.roomInfo.roomName;
    clearPlayerList();
    for (const auto &playerName : data.playerNames) {
        addPlayerToList(playerName);
    }
    ui->room_name->setText("Room Name: " + roomName);


    // Enable the launch game button if the current player is the host (first player in the list)
    if (!data.playerNames.empty() && data.playerNames[0] == playerNames[0]) {
        ui->launchGameButton->setEnabled(true);
    } else {
        ui->launchGameButton->setEnabled(false);
    }
}

void GameLobby::addPlayerToList(const QString &playerName) {
    ui->players_list->addItem(playerName);
    playerNames.push_back(playerName);
}

void GameLobby::clearPlayerList() {
    QListWidgetItem *child;
    while ((child = ui->players_list->takeItem(0)) != nullptr) {
        delete child;
    }
    playerNames.clear();
}

void GameLobby::removePlayerFromList(const QString &playerName) {
    for (int i = 0; i < ui->players_list->count(); ++i) {
        QListWidgetItem *item = ui->players_list->takeItem(i);
        if (item && item->text() == playerName) {
            delete item;
            playerNames.erase(std::remove(playerNames.begin(), playerNames.end(), playerName), playerNames.end());
            break;
        }
    }
}

GameLobby::~GameLobby() {
    delete ui;
}