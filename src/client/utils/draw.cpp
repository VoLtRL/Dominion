#include "client/utils/draw.hpp"
#include "client/VisualCard.hpp"

VisualCard* drawCard(QGraphicsScene *scene, const Card* card, int x, int y) {
    VisualCard *visualCard = new VisualCard(VisualCard::getNewId(), QString::fromStdString(card->getName()), card->getCost(), QString(card->getPath().c_str()), Location::DECK);
    visualCard->setPos(x, y);
    scene->addItem(visualCard);
    return visualCard;
}

std::vector<VisualCard*> drawPlayerHand(QGraphicsScene *scene, const std::vector<const Card*> hand) {
    // clear the previous hand cards from the scene
    QList<QGraphicsItem*> items = scene->items();
    std::vector<VisualCard*> visualCardsInHand;
    for (QGraphicsItem* item : items) {
        VisualCard* visualCard = dynamic_cast<VisualCard*>(item);
        if (visualCard && visualCard->getLocation() == Location::HAND) { // ça marche pas, il faut que je trouve un moyen de savoir si la carte est dans la main du joueur
            scene->removeItem(visualCard);
            delete visualCard;
        }
    }

    // draw the cards in the player's hand at the bottom of the scene
    int x = 10; // Starting x position
    int y = 400;

    for (const Card* card : hand) {
        visualCardsInHand.push_back(drawCard(scene, card, x, y));
        x += 110; // Move to the right for the next card
    }
    return visualCardsInHand;
}

std::vector<VisualCard*> drawCardsInPlay(QGraphicsScene *scene, const std::vector<const Card*> cardsInPlay) {
    int x = 10;
    int y = 200;
    std::vector<VisualCard*> visualCardsInPlay;
    for (const Card* card : cardsInPlay) {
        visualCardsInPlay.push_back(drawCard(scene, card, x, y));
        x += 90; // Move to the right for the next card
    }
    return visualCardsInPlay;
}