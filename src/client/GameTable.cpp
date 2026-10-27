#include "client/GameTable.hpp"
#include "client/utils/draw.hpp"
#include "shared/gameplay/Factory.hpp"

#include "ui_GameTable.h"

#include <QResizeEvent>

GameTable::GameTable(QWidget *parent) : QWidget(parent), ui(new Ui::GameTable) {
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->gameView->setScene(scene);
    scene->setSceneRect(ui->gameView->viewport()->rect());

    connect(ui->endTurnButton, &QPushButton::clicked, this, &GameTable::onEndTurnClicked);

}

void GameTable::onEndTurnClicked() {
    emit endTurnClicked();

    const std::vector<const Card*> hand = {
        Factory::get("Copper"),
        Factory::get("Estate"),
        Factory::get("Silver"),
        Factory::get("Duchy"),
        Factory::get("Gold")
    };
    const std::vector<const Card*> cardsInPlay = {
        Factory::get("Village"),
        Factory::get("Smithy"),
        Factory::get("Market")
    };

    constexpr qreal cardWidth = 100;
    constexpr qreal cardHeight = 150;
    constexpr qreal cardSpacing = 110;
    constexpr qreal bottomMargin = 20;
    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    const qreal handWidth = cardWidth + (hand.size() - 1) * cardSpacing;
    drawPlayerHand(
        hand,
        (sceneWidth - handWidth) / 2,
        sceneHeight - cardHeight - bottomMargin);

    const qreal cardsInPlayWidth = cardWidth + (cardsInPlay.size() - 1) * cardSpacing;
    drawCardsInPlay(
        cardsInPlay,
        (sceneWidth - cardsInPlayWidth) / 2,
        sceneHeight - cardHeight - bottomMargin - 200);

    drawStacks(10, 5);
}

void GameTable::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    scene->setSceneRect(ui->gameView->viewport()->rect());
}


void GameTable::drawPlayerHand(const std::vector<const Card*> hand, qreal x, qreal y) {
    visualCardsInHand.clear();
    std::vector<VisualCard*> visualCards = ::drawPlayerHand(scene, hand, x, y);
    for (VisualCard* visualCard : visualCards) {
        connect(visualCard, &VisualCard::clicked, [this, visualCard](int cardId) {
            visualCard->setSelectedEffect(!visualCard->isSelected());
            visualCardsInHand.push_back(visualCard);
        });
    }
}

void GameTable::drawCardsInPlay(const std::vector<const Card*> cardsInPlay, qreal x, qreal y) {
    visualCardsInPlay.clear();
    const QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem *item : items) {
        VisualCard *visualCard = dynamic_cast<VisualCard*>(item);
        if (visualCard && visualCard->getLocation() == Location::IN_PLAY) {
            scene->removeItem(visualCard);
            delete visualCard;
        }
    }

    std::vector<VisualCard*> visualCards = ::drawCardsInPlay(scene, cardsInPlay, x, y);
    for (VisualCard* visualCard : visualCards) {
        connect(visualCard, &VisualCard::clicked, [this, visualCard](int cardId) {
            visualCard->setSelectedEffect(!visualCard->isSelected());
            visualCardsInPlay.push_back(visualCard);
        });
    }
}

void GameTable::drawStacks(quint16 cardsInDraw, quint16 cardsInDiscard){
    constexpr qreal cardWidth = 100;
    constexpr qreal cardHeight = 150;
    const qreal drawX = 20;
    const qreal discardX = scene->sceneRect().width() - cardWidth - 20;
    const qreal stackY = scene->sceneRect().height() - cardHeight - 20;

    drawBackCard(scene, drawX, stackY);
    drawBackCard(scene, discardX, stackY);

    QGraphicsTextItem* drawText = scene->addText(QString::number(cardsInDraw));
    drawText->setPos(drawX, stackY + cardHeight + 2);
    QGraphicsTextItem* discardText = scene->addText(QString::number(cardsInDiscard));
    discardText->setPos(discardX, stackY + cardHeight + 2);
    drawText->setDefaultTextColor(Qt::white);
    discardText->setDefaultTextColor(Qt::white);
}

void GameTable::displayMessage(QString message){
    scene->addText(message);
}

GameTable::~GameTable() {
    delete ui;
}