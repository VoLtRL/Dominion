#pragma once
#include <QWidget>
#include "shared/structs/Payloads.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class JoinRoom; }
QT_END_NAMESPACE

class JoinRoom : public QWidget {
    Q_OBJECT

private:
    Ui::JoinRoom *ui;
    std::vector<Network::RoomInfo> availableRooms; // Vector to store available games with their names and IDs

signals:
    void goBackToMenuClicked();
    void joinRoomClicked(int roomId);

private slots:
    void onJoinRoomClicked();
    void onGoBackToMenuClicked();

public:
    explicit JoinRoom(QWidget *parent = nullptr);
    ~JoinRoom() override;
    void updateAvailableRooms(const std::vector<Network::RoomInfo>& rooms);

};