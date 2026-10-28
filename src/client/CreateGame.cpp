#include "client/CreateGame.hpp"

#include "ui_CreateGame.h"

CreateGame::CreateGame(QWidget *parent) : QWidget(parent), ui(new Ui::CreateGame) {
    ui->setupUi(this);

    connect(ui->goBackToMenuButton, &QPushButton::clicked, this, &CreateGame::onGoBackToMenuClicked);
    connect(ui->createRoomButton, &QPushButton::clicked, this, &CreateGame::onCreateRoomClicked);

    for (const auto &gameMode : PredefinedGameModes::getGameModes()) {
        ui->GameModeList->addItem(QString::fromStdString(gameMode.getName()));
    }
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

Network::CreateRoomPayload CreateGame::getRoomData() const {

    GameMode gameMode = PredefinedGameModes::getGameModes().front();

    if(!(ui->GameModeList->currentRow() < 0) || !(ui->GameModeList->currentRow() >= PredefinedGameModes::getGameModes().size())) {
        gameMode = PredefinedGameModes::getGameModes()[ui->GameModeList->currentRow()];
    }

    return Network::CreateRoomPayload(
        "PlaceholderPlayer",
        getRoomName(),
        gameMode,
        getNumBots().toInt(),
        getNumPlayers().toInt()
    );
}

