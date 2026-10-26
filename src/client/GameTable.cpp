#include "client/GameTable.hpp"
#include "client/utils/draw.hpp"
#include "shared/gameplay/Factory.hpp"

#include "ui_GameTable.h"

GameTable::GameTable(QWidget *parent) : QWidget(parent), ui(new Ui::GameTable) {
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->gameView->setScene(scene);

    connect(ui->endTurnButton, &QPushButton::clicked, this, &GameTable::onEndTurnClicked);

}

void GameTable::onEndTurnClicked() {
    emit endTurnClicked();
    drawPlayerHand({Factory::get("Copper"), Factory::get("Estate"), Factory::get("Silver"), Factory::get("Duchy"), Factory::get("Gold")});
}

void GameTable::drawPlayerHand(const std::vector<const Card*> hand) {
    std::vector<VisualCard*> visualCards = ::drawPlayerHand(scene, hand);
    for (VisualCard* visualCard : visualCards) {
        connect(visualCard, &VisualCard::clicked, [this, visualCard](int cardId) {
            visualCard->setSelectedEffect(!visualCard->isSelected());
            visualCardsInHand.push_back(visualCard);
        });
    }
}

GameTable::~GameTable() {
    delete ui;
}