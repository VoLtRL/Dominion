#include "client/MainMenu.hpp"

#include "ui_MainMenu.h"

MainMenu::MainMenu(QWidget *parent) : QWidget(parent), ui(new Ui::MainMenu) {
    ui->setupUi(this);

    connect(ui->createGameButton, &QPushButton::clicked, this, &MainMenu::onCreateGameClicked);
    connect(ui->joinGameButton, &QPushButton::clicked, this, &MainMenu::onJoinGameClicked);
    connect(ui->leaderboardButton, &QPushButton::clicked, this, &MainMenu::onLeaderboardClicked);
    connect(ui->quitButton, &QPushButton::clicked, this, &MainMenu::onQuitClicked);
}

MainMenu::~MainMenu() {
    delete ui;
}

void MainMenu::onCreateGameClicked() {
    emit createGameClicked();
}

void MainMenu::onJoinGameClicked() {
    emit joinGameClicked();
}

void MainMenu::onLeaderboardClicked() {
    emit leaderboardClicked();
}

void MainMenu::onQuitClicked() {
    emit quitClicked();
}