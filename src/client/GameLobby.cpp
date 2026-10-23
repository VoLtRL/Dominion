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

void GameLobby::updateRoomInfo(const Network::RoomCreatedPayload &data) {
    QString roomName = data.value("roomName").toString();
    int botCount = data.value("botCount").toInt();
    int playerMaxCount = data.value("playerMaxCount").toInt();

    ui->roomNameLabel->setText("Room Name: " + roomName);
    ui->botCountLabel->setText("Bot Count: " + QString::number(botCount));
    ui->playerMaxCountLabel->setText("Max Players: " + QString::number(playerMaxCount));
}

GameLobby::~GameLobby() {
    delete ui;
}