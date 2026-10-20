#include "client/CreateGame.hpp"

#include "ui_CreateGame.h"

CreateGame::CreateGame(QWidget *parent) : QWidget(parent), ui(new Ui::CreateGame) {
    ui->setupUi(this);

    connect(ui->goBackToMenuButton, &QPushButton::clicked, this, &CreateGame::onGoBackToMenuClicked);
    connect(ui->createRoomButton, &QPushButton::clicked, this, &CreateGame::onCreateRoomClicked);
}

void CreateGame::onGoBackToMenuClicked() {
    emit goBackToMenuClicked();
}

void CreateGame::onCreateRoomClicked() {
    emit createRoomClicked();
}

CreateGame::~CreateGame() {
    delete ui;
}

QString CreateGame::getRoomName() const {
    if (ui->roomName->text().isEmpty()) {
        return "Room_" + QString::number(QRandomGenerator::global()->bounded(10000));
    }
    return ui->roomName->text();
}

QString CreateGame::getNumPlayers() const {
    if (ui->player_1->isChecked()) return "1";
    if (ui->player_2->isChecked()) return "2";
    if (ui->player_3->isChecked()) return "3";
    if (ui->player_4->isChecked()) return "4";
    return "";
}

QString CreateGame::getNumBots() const {
    if (ui->bot_1->isChecked()) return "1";
    if (ui->bot_2->isChecked()) return "2";
    if (ui->bot_3->isChecked()) return "3";
    if (ui->bot_4->isChecked()) return "4";
    return "";
}


QJsonObject CreateGame::getRoomData() const {
    QJsonObject roomData;
    roomData["name"] = getRoomName();
    roomData["numPlayers"] = getNumPlayers();
    roomData["numBots"] = getNumBots();
    return roomData;
}

