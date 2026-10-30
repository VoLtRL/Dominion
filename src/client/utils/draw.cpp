#include "client/utils/draw.hpp"
#include "client/VisualCard.hpp"

    
VisualCard* drawCard(QGraphicsScene *scene, const Card* card, qreal x, qreal y, Location location) {
    VisualCard *visualCard = new VisualCard(
        VisualCard::getNewId(),
        card,
        QString::fromStdString(card->getName()),
        card->getCost(),
        QString(card->getPath().c_str()),
        location);
    visualCard->setPos(x, y);
    scene->addItem(visualCard);
    return visualCard;
}

std::vector<VisualCard*> drawPlayerHand(QGraphicsScene *scene, const std::vector<const Card*> hand, qreal x, qreal y) {
    // clear the previous hand cards from the scene
    QList<QGraphicsItem*> items = scene->items();
    std::vector<VisualCard*> visualCardsInHand;
    for (QGraphicsItem* item : items) {
        VisualCard* visualCard = dynamic_cast<VisualCard*>(item);
        if (visualCard && visualCard->data(0) == "hand-card") {
            scene->removeItem(visualCard);
            delete visualCard;
        }
    }

    for (const Card* card : hand) {
        VisualCard* visualCard = drawCard(scene, card, x, y, Location::HAND);
        visualCard->setData(0, "hand-card"); // this is a hand card.
        visualCardsInHand.push_back(visualCard);
        x += 110; // Move to the right for the next card
    }
    return visualCardsInHand;
}

std::vector<VisualCard*> drawCardsInPlay(QGraphicsScene *scene, const std::vector<const Card*> cardsInPlay, qreal x, qreal y) {
    std::vector<VisualCard*> visualCardsInPlay;
    for (const Card* card : cardsInPlay) {
        visualCardsInPlay.push_back(drawCard(scene, card, x, y, Location::IN_PLAY));
        x += 90; // Move to the right for the next card
    }
    return visualCardsInPlay;
}


QGraphicsPixmapItem* drawBackCard(QGraphicsScene *scene, qreal x, qreal y, int width, int height) {
    QPixmap backPixmap(":/images/back_card.png");

    backPixmap = backPixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QGraphicsPixmapItem* backCard = new QGraphicsPixmapItem(backPixmap);
    
    backCard->setPos(x,y);
    scene->addItem(backCard);
    return backCard;
}

void drawStack(QGraphicsScene *scene, qreal x, qreal y,int rotation, quint16 count) {
    constexpr int cardWidth = 100;
    constexpr int cardHeight = 150;

    drawBackCard(scene, x, y, cardWidth, cardHeight);
    QGraphicsItem *stack = scene->items().front();
    stack->setRotation(rotation);
    stack->setData(0, "other-player-stack");

    QGraphicsTextItem *countText = scene->addText(QString::number(count));
    countText->setPos(x, y + cardHeight + 2);
    countText->setData(0, "other-player-stack");
}