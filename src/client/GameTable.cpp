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

void GameTable::updateGameState(ClientGameState newGameState) {
    gameState = newGameState;

    drawTable();
}

void GameTable::drawTable() {
    // Clear the scene
    const QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem *item : items) {
        scene->removeItem(item);
        delete item;
    }

    constexpr qreal cardWidth = 100;
    constexpr qreal cardHeight = 150;
    constexpr qreal cardSpacing = 110;
    constexpr qreal bottomMargin = 20;
    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    const qreal handWidth = cardWidth + (gameState.hand.getCards().size() - 1) * cardSpacing;

    // Draw the table
    drawPlayerHand(gameState.hand.getCards(), (sceneWidth - handWidth) / 2, sceneHeight - cardHeight - bottomMargin);
    drawStacks(gameState.selfDrawStackSize, gameState.selfDiscardStackSize, gameState.selfTopDiscardCard);
    drawOtherPlayersHands(gameState.otherPlayers);
    drawOtherPlayersDrawAndDiscard(gameState.otherPlayers);
    drawOtherCardsInPlay(gameState.otherPlayers);
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

    drawTable();
}

void GameTable::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    scene->setSceneRect(ui->gameView->viewport()->rect());
    emit widgetResized(event->size());}

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

void GameTable::drawOtherPlayersHands(const std::vector<ClientPlayerState>& players) {
        constexpr qreal cardWidth = 100;
        constexpr qreal cardHeight = 150;
        constexpr qreal cardSpacing = 110;
        constexpr qreal margin = 20;
        const qreal sceneWidth = scene->sceneRect().width();
        const qreal sceneHeight = scene->sceneRect().height();

        const QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem *item : items) {
            VisualCard *visualCard = dynamic_cast<VisualCard*>(item);
            if (visualCard && visualCard->getLocation() == Location::OTHER_PLAYER_HAND) {
                scene->removeItem(visualCard);
                delete visualCard;
            }
        }

        const auto drawOtherHand = [&](int numberOfCards, qreal centerX, qreal centerY,
                                       qreal rotation) {
            if (numberOfCards <= 0) {
                return;
            }

            const qreal handLength = cardWidth + (numberOfCards - 1) * cardSpacing;
            for (size_t cardIndex = 0; cardIndex < numberOfCards; ++cardIndex) {
                QGraphicsPixmapItem* backCard = drawBackCard(scene,centerX + (cardIndex * cardSpacing - handLength / 2 + cardWidth / 2), centerY - cardHeight / 2); // Draw the back of the card
                backCard->setTransformOriginPoint(cardWidth / 2, cardHeight / 2);
                backCard->setRotation(rotation);
                scene->addItem(backCard);
            }
        };

        for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex) {
            const ClientPlayerState *player = &players[playerIndex];
            if (!player) {
                continue;
            }

            if (playerIndex == 0) {
                drawOtherHand(player->handSize, sceneWidth / 2, margin + cardHeight / 2, 180);
            } else if (playerIndex == 1) {
                drawOtherHand(player->handSize, margin + cardWidth / 2,
                              sceneHeight / 2, 90);
            } else if (playerIndex == 2) {
                drawOtherHand(player->handSize, sceneWidth - margin - cardWidth / 2,
                              sceneHeight / 2, -90);
            }
        }
}

void GameTable::drawOtherCardsInPlay(
        const std::vector<ClientPlayerState>& players) {
        constexpr qreal cardWidth = 100;
        constexpr qreal cardHeight = 150;
        constexpr qreal cardSpacing = 90;
        constexpr qreal margin = 20;
        const qreal sceneWidth = scene->sceneRect().width();
        const qreal sceneHeight = scene->sceneRect().height();

        const QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem *item : items) {
            VisualCard *visualCard = dynamic_cast<VisualCard*>(item);
            if (visualCard && visualCard->getLocation() == Location::OTHER_PLAYER_IN_PLAY) {
                scene->removeItem(visualCard);
                delete visualCard;
            }
        }

        for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex) {
            const auto &cards = players[playerIndex].cardsInPlay;
            const qreal cardsLength =
                cardWidth + (cards.size() > 0 ? (cards.size() - 1) * cardSpacing : 0);

            qreal centerX;
            qreal centerY;
            qreal rotation;
            if (playerIndex == 0) {
                centerX = sceneWidth / 2;
                centerY = margin + cardHeight * 1.5;
                rotation = 180;
            } else if (playerIndex == 1) {
                centerX = margin + cardWidth / 2 + cardHeight / 2;
                centerY = sceneHeight / 2;
                rotation = 90;
            } else if (playerIndex == 2) {
                centerX = sceneWidth - margin - cardWidth / 2 - cardHeight / 2;
                centerY = sceneHeight / 2;
                rotation = -90;
            } else {
                continue;
            }

            for (size_t cardIndex = 0; cardIndex < cards.size(); ++cardIndex) {
                const Card *card = cards[cardIndex];
                VisualCard *visualCard = new VisualCard(
                    VisualCard::getNewId(),
                    QString::fromStdString(card->getName()),
                    card->getCost(),
                    QString(card->getPath().c_str()),
                    Location::OTHER_PLAYER_IN_PLAY);
                const qreal offset = cardIndex * cardSpacing - cardsLength / 2 + cardWidth / 2;
                visualCard->setTransformOriginPoint(cardWidth / 2, cardHeight / 2);
                visualCard->setPos(centerX + offset - cardWidth / 2,
                                   centerY - cardHeight / 2);
                visualCard->setRotation(rotation);
                scene->addItem(visualCard);
            }
        }
}

void GameTable::drawOtherPlayersDrawAndDiscard(
        const std::vector<ClientPlayerState>& players) {
        constexpr qreal cardWidth = 100;
        constexpr qreal cardHeight = 150;
        constexpr qreal margin = 20;
        const qreal sceneWidth = scene->sceneRect().width();
        const qreal sceneHeight = scene->sceneRect().height();

        const QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem *item : items) {
            if (item->data(0).toString() == "other-player-stack") {
                scene->removeItem(item);
                delete item;
            }
        }

        for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex) {
            const ClientPlayerState *player = &players[playerIndex];
            if (!player || playerIndex > 2) {
                continue;
            }

            qreal drawX;
            qreal drawY;
            qreal discardX;
            qreal discardY;
            qreal rotation;
            if (playerIndex == 0) {
                drawX = sceneWidth / 2 - cardWidth - margin;
                drawY = margin;
                discardX = sceneWidth / 2 + margin;
                discardY = margin;
                rotation = 180;
            } else if (playerIndex == 1) {
                drawX = margin;
                drawY = sceneHeight / 2 - cardHeight - margin;
                discardX = margin;
                discardY = sceneHeight / 2 + margin;
                rotation = 90;
            } else {
                drawX = sceneWidth - cardWidth - margin;
                drawY = sceneHeight / 2 - cardHeight - margin;
                discardX = sceneWidth - cardWidth - margin;
                discardY = sceneHeight / 2 + margin;
                rotation = -90;
            }

            drawStack(scene, drawX, drawY, rotation, player->drawStackSize);
            drawStack(scene, discardX, discardY, rotation, player->discardStackSize);
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

void GameTable::drawStacks(quint16 cardsInDraw, quint16 cardsInDiscard, const Card* topDiscardCard) {
    constexpr qreal cardWidth = 100;
    constexpr qreal cardHeight = 150;
    const qreal drawX = 20;
    const qreal discardX = scene->sceneRect().width() - cardWidth - 20;
    const qreal stackY = scene->sceneRect().height() - cardHeight - 20;

    drawBackCard(scene, drawX, stackY);
    if (topDiscardCard) {
        drawCard(scene,topDiscardCard, discardX, stackY);
    } else {
        drawBackCard(scene, discardX, stackY);
    }

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