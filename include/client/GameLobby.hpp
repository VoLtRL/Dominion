#pragma once

#include <QWidget>
#include "shared/structs/Payloads.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class GameLobby; }
QT_END_NAMESPACE

class GameLobby : public QWidget {
    Q_OBJECT
private:
    Ui::GameLobby *ui;
    int roomId;
    int botCount;
    int playerMaxCount;
    std::vector<QString> playerNames;

signals:
    void goBackToMenuClicked();
    void launchGameClicked();

private slots:
    void onGoBackToMenuClicked();
    void onLaunchGameClicked();

public:
    explicit GameLobby(QWidget *parent = nullptr);
    ~GameLobby() override;
    void updateRoomInfo(const Network::CreatedRoomPayload &data);
    void updateRoomInfo(const Network::RoomJoinedPayload &data);
    void addPlayerToList(const QString &playerName);
    void clearPlayerList();
    void removePlayerFromList(const QString &playerName);
};