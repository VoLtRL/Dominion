#pragma once

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QBrush>
#include <QPen>

#include <vector>

#include "shared/abstract/Card.hpp"

void drawCard(QGraphicsScene *scene, const Card* card, int x, int y);

void drawPlayerHand(QGraphicsScene *scene, const std::vector<const Card*> hand);

void drawCardsInPlay(QGraphicsScene *scene, const std::vector<const Card*> cardsInPlay);

