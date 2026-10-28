#pragma once

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QBrush>
#include <QPen>

#include <vector>

#include "shared/abstract/Card.hpp"
#include "client/VisualCard.hpp"

VisualCard* drawCard(
    QGraphicsScene *scene,
    const Card* card,
    qreal x,
    qreal y,
    Location location = Location::DECK);

std::vector<VisualCard*> drawPlayerHand(QGraphicsScene *scene, const std::vector<const Card*> hand, qreal x, qreal y);

std::vector<VisualCard*> drawCardsInPlay(QGraphicsScene *scene, const std::vector<const Card*> cardsInPlay, qreal x, qreal y);

QGraphicsPixmapItem* drawBackCard(QGraphicsScene *scene, qreal x, qreal y, int width = 100, int height = 150);

void drawStack(QGraphicsScene *scene, qreal x, qreal y,int rotation, quint16 count);