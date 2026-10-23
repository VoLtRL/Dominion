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

signals:
    void goBackToMenuClicked();
    void launchGameClicked();

private slots:
    void onGoBackToMenuClicked();
    void onLaunchGameClicked();

public:
    explicit GameLobby(QWidget *parent = nullptr);
    ~GameLobby() override;

    void updateRoomInfo(const Network::RoomCreatedPayload &data);
};