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

GameLobby::~GameLobby() {
    delete ui;
}