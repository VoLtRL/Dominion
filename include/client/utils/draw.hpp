#pragma once

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QBrush>
#include <QPen>

#include <vector>

#include "shared/abstract/Card.hpp"
#include "client/VisualCard.hpp"

VisualCard* drawCard(QGraphicsScene *scene, const Card* card, int x, int y);

std::vector<VisualCard*> drawPlayerHand(QGraphicsScene *scene, const std::vector<const Card*> hand);

std::vector<VisualCard*> drawCardsInPlay(QGraphicsScene *scene, const std::vector<const Card*> cardsInPlay);

