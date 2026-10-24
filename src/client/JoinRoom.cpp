#include "client/JoinRoom.hpp"
#include "shared/structs/Payloads.hpp"
#include "ui_JoinRoom.h"

JoinRoom::JoinRoom(QWidget *parent) : QWidget(parent), ui(new Ui::JoinRoom) {
    ui->setupUi(this);

    connect(ui->joinRoomButton, &QPushButton::clicked, this, &JoinRoom::onJoinRoomClicked);
    connect(ui->goBackToMenuButton, &QPushButton::clicked, this, &JoinRoom::onGoBackToMenuClicked);
}

JoinRoom::~JoinRoom() {
    delete ui;
}

void JoinRoom::updateAvailableRooms(const std::vector<Network::RoomInfo>& rooms) {
    availableRooms = rooms;
    ui->availableRoomsList->clear();
    for (const auto& room : rooms) {
        QString roomText = QString("%1 (ID: %2, Places left: %3)").arg(room.roomName).arg(room.roomId).arg(room.placesLeft);
        ui->availableRoomsList->addItem(roomText);
        availableRooms.push_back(room);
    }
}

void JoinRoom::onJoinRoomClicked() {
    if (ui->availableRoomsList->currentItem()) {
        int roomId = availableRooms[ui->availableRoomsList->currentRow()].roomId;
        emit joinRoomClicked(roomId);
    }
}

void JoinRoom::onGoBackToMenuClicked() {
    emit goBackToMenuClicked();
}

