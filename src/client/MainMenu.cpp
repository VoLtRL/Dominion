#include "client/MainMenu.hpp"
#include "client/SettingsDialog.hpp"

#include "ui_MainMenu.h"

MainMenu::MainMenu(QWidget *parent) : QWidget(parent), ui(new Ui::MainMenu) {
    ui->setupUi(this);

    settingsDialog = new SettingsDialog(this);

    connect(ui->createGameButton, &QPushButton::clicked, this, &MainMenu::onCreateGameClicked);
    connect(ui->joinRoomButton, &QPushButton::clicked, this, &MainMenu::onJoinRoomClicked);
    connect(ui->leaderboardButton, &QPushButton::clicked, this, &MainMenu::onLeaderboardClicked);
    connect(ui->quitButton, &QPushButton::clicked, this, &MainMenu::onQuitClicked);

    connect(ui->settingsButton, &QPushButton::clicked, this, &MainMenu::onSettingsButtonClicked);
    connect(settingsDialog, &SettingsDialog::settingsChanged,
            this, &MainMenu::settingsChanged);
}

MainMenu::~MainMenu() {
    delete ui;
}

void MainMenu::setSettings(const QString &playerName, const QString &hostAddress, quint16 hostPort) {
    settingsDialog->setValues(playerName, hostAddress, hostPort);
}

void MainMenu::onSettingsButtonClicked() {
    settingsDialog->exec();
}

void MainMenu::onCreateGameClicked() {
    emit createGameClicked();
}

void MainMenu::onJoinRoomClicked() {
    emit joinRoomClicked();
}

void MainMenu::onLeaderboardClicked() {
    emit leaderboardClicked();
}

void MainMenu::onQuitClicked() {
    emit quitClicked();
}