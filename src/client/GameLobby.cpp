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

void GameLobby::updateRoomInfo() {
    ui->players_list->clear();
    for (int i = 0; i < playerNames.size(); ++i) {
        const QString &name = playerNames[i];
        ui->players_list->addItem(name + (i == 0 ? " (Host)" : ""));
    }
    ui->room_name->setText("Room Name: " + roomName);


    const bool isHost = !playerNames.empty() && playerNames.front() == currentPlayerName;
    if (isHost) {
        ui->launchGameButton->setEnabled(true);
        ui->launchGameButton->setToolTip({});
    } else {
        ui->launchGameButton->setEnabled(false);
        ui->launchGameButton->setToolTip(
            "Only the host can launch the game. The host is " + playerNames.front());
    }
}

void GameLobby::setCurrentPlayerName(const QString &playerName) {
    currentPlayerName = playerName;
}

void GameLobby::setRoomName(const QString &name) {
    roomName = name;
}

void GameLobby::addPlayerToList(const QString &playerName) {
    playerNames.push_back(playerName);
    ui->players_list->clear();
    for (int i = 0; i < playerNames.size(); ++i) {
        const QString &name = playerNames[i];
        ui->players_list->addItem(name + (i == 0 ? " (Host)" : ""));
    }
}

void GameLobby::clearPlayerList() {
    ui->players_list->clear();
    playerNames.clear();
}

void GameLobby::removePlayerFromList(const QString &playerName) {
    auto it = std::find(playerNames.begin(), playerNames.end(), playerName);
    if (it != playerNames.end()) {
        playerNames.erase(it);
    }
}

GameLobby::~GameLobby() {
    delete ui;
}