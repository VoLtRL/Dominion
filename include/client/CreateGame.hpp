#pragma once

#include <QWidget>
#include <QRandomGenerator>
#include <QJsonObject>

#include "shared/structs/Payloads.hpp"
#include "shared/gameplay/GameMode.hpp"
#include "client/PredefinedGameModes.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class CreateGame; }
QT_END_NAMESPACE

class CreateGame : public QWidget {
    Q_OBJECT
private:
    Ui::CreateGame *ui;

signals:
    void goBackToMenuClicked();
    void createRoomClicked();

private slots:
    void onGoBackToMenuClicked();
    void onCreateRoomClicked();

public:
    explicit CreateGame(QWidget *parent = nullptr);
    ~CreateGame() override;

    QString getRoomName() const;

    QString getNumPlayers() const ;

    QString getNumBots() const ;

    Network::CreateRoomPayload getRoomData() const;
};
