#include "client/utils/draw.hpp"

void drawCard(QGraphicsScene *scene, const Card* card, int x, int y) {
    // Placeholder implementation for drawing a card
    // In a real implementation, you would use QGraphicsPixmapItem or similar to draw the card image
    QGraphicsRectItem *rect = scene->addRect(x, y, 100, 150); // Example dimensions for a card
    rect->setBrush(QBrush(Qt::white));
    rect->setPen(QPen(Qt::black));
}

void drawPlayerHand(QGraphicsScene *scene, const std::vector<const Card*> hand) {
    int x = 10;
    int y = 10;
    for (const Card* card : hand) {
        drawCard(scene, card, x, y);
        x += 110; // Move to the right for the next card
    }
}

void drawCardsInPlay(QGraphicsScene *scene, const std::vector<const Card*> cardsInPlay) {
    int x = 10;
    int y = 200;
    for (const Card* card : cardsInPlay) {
        drawCard(scene, card, x, y);
        x += 110; // Move to the right for the next card
    }
}