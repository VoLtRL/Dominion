#include "client/GameTable.hpp"
#include "client/utils/draw.hpp"
#include "shared/gameplay/Factory.hpp"

#include "ui_GameTable.h"

#include <QResizeEvent>
#include <QGraphicsTextItem>

namespace {
    constexpr qreal CardWidth = 100;
    constexpr qreal CardHeight = 150;
    constexpr qreal CardSpacing = 110;
    constexpr qreal TableMargin = 20;

    void setRotatedItemGeometry(QGraphicsItem *item, qreal centerX, qreal centerY, qreal rotation) {
        item->setTransformOriginPoint(CardWidth / 2, CardHeight / 2);
        item->setPos(centerX - CardWidth / 2, centerY - CardHeight / 2);
        item->setRotation(rotation);
    }
}

GameTable::GameTable(QWidget *parent) : QWidget(parent), ui(new Ui::GameTable) {
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->gameView->setScene(scene);
    scene->setSceneRect(ui->gameView->viewport()->rect());

    connect(ui->endTurnButton, &QPushButton::clicked, this, &GameTable::onEndTurnClicked);
    connect(ui->playSelectedButton, &QPushButton::clicked, [this]() {
        for (VisualCard* visualCard : visualCardsInHand) {
            if (visualCard->isSelected()) {
                emit visualCard->clicked(visualCard->getId());
                visualCard->setSelectedEffect(false);
            }
        }
    });

    connect(ui->endPhaseButton, &QPushButton::clicked, [this]() {
    });


    connect(ui->switchShopButton, &QPushButton::clicked, this, &GameTable::switchShop);

}

void GameTable::startTurn() {
    bool noActivePlayer = true;
    setPhaseText(gameState.currentPhase);
    for(const ClientPlayerState& player : gameState.otherPlayers) {
        if(player.isActive) {
            displayMessage(QString("%1's turn").arg(player.name));
            noActivePlayer = false;
            break;
        }
    }
    if(noActivePlayer) {
        displayMessage("It's your turn");
        modifySelection(gameState.currentPhase, gameState.currentPlayerActions, gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);
    }

    switchShop();
}

void GameTable::changePhase(QString newPhase) {
    gameState.currentPhase = newPhase;
    
    modifySelection(gameState.currentPhase, gameState.currentPlayerActions, gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);
}

void GameTable::switchShop() {
    if (isShopVisible) {
        const QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem *item : items) {
            if (item->data(0).toString() == "shop-card") {
                scene->removeItem(item);
                delete item;
            }
        }
        isShopVisible = false;
    }else{

        std::cout << "Drawing shop with " << gameState.shop.size() << " cards." << std::endl;
        const qreal sceneWidth = scene->sceneRect().width();
        const qreal sceneHeight = scene->sceneRect().height();
        const qreal shopWidth = gameState.shop.empty() ? 0 : CardWidth + (gameState.shop.size() - 1) * CardSpacing;
        const qreal shopHeight = CardHeight * 2 + CardSpacing;
        drawShop((sceneWidth - shopWidth) / 2, (sceneHeight - shopHeight) / 2);
        isShopVisible = true;

    }


    isShopVisible = !isShopVisible;
}

void GameTable::modifySelection(QString phase, int actions, int purchases, int money) {
    ui->playSelectedButton->setEnabled(phase == "Action" && actions > 0);
    ui->endTurnButton->setEnabled(phase == "Buy" && purchases > 0);

    setPhaseText(phase);
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

    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    qDebug() << "Scene width: " << sceneWidth << ", Scene height: " << sceneHeight;

    const qreal handWidth = gameState.hand.getCards().empty() ? 0 : CardWidth + (gameState.hand.getCards().size() - 1) * CardSpacing;

    // Draw the table
    drawPlayerHand(gameState.hand.getCards(), (sceneWidth - handWidth) / 2, sceneHeight - CardHeight - TableMargin);
    drawStacks(gameState.selfDrawStackSize, gameState.selfDiscardStackSize, gameState.selfTopDiscardCard);
    const qreal cardsInPlayWidth = gameState.cardsInPlay.empty() ? 0 : CardWidth + (gameState.cardsInPlay.size() - 1) * 90;
    drawCardsInPlay(gameState.cardsInPlay, (sceneWidth - cardsInPlayWidth) / 2, sceneHeight / 2 - CardHeight / 2);
    drawOtherPlayersHands(gameState.otherPlayers);
    drawOtherPlayersDrawAndDiscard(gameState.otherPlayers);
    drawOtherCardsInPlay(gameState.otherPlayers);
}

void GameTable::onEndTurnClicked() {
    emit endTurnClicked();
}

void GameTable::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    scene->setSceneRect(ui->gameView->viewport()->rect());
    drawTable();
    emit widgetResized(event->size());
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

void GameTable::drawOtherPlayersHands(const std::vector<ClientPlayerState>& players) {
        const qreal sceneWidth = scene->sceneRect().width();
        const qreal sceneHeight = scene->sceneRect().height();

        const QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem *item : items) {
            if (item->data(0).toString() == "other-player-hand") {
                scene->removeItem(item);
                delete item;
            }
        }

        const auto drawOtherHand = [&](int numberOfCards, qreal centerX, qreal centerY,
                                       bool vertical,
                                       qreal rotation) {
            if (numberOfCards <= 0) {
                return;
            }

            const qreal handLength = CardWidth + (numberOfCards - 1) * CardSpacing;
            for (size_t cardIndex = 0; cardIndex < numberOfCards; ++cardIndex) {
                const qreal offset = cardIndex * CardSpacing - handLength / 2 + CardWidth / 2;
                const qreal cardCenterX = vertical ? centerX : centerX + offset;
                const qreal cardCenterY = vertical ? centerY + offset : centerY;
                QGraphicsPixmapItem *backCard = drawBackCard(
                    scene, cardCenterX - CardWidth / 2, cardCenterY - CardHeight / 2);
                backCard->setData(0, "other-player-hand");
                setRotatedItemGeometry(backCard, cardCenterX, cardCenterY, rotation);
            }
        };

        for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex) {
            const ClientPlayerState *player = &players[playerIndex];
            if (!player) {
                continue;
            }

            if (playerIndex == 0) {
                drawOtherHand(player->handSize, sceneWidth / 2,
                              TableMargin + CardHeight / 2, false, 180);
            } else if (playerIndex == 1) {
                drawOtherHand(player->handSize, TableMargin + CardHeight / 2,
                              sceneHeight / 2, true, 90);
            } else if (playerIndex == 2) {
                drawOtherHand(player->handSize,
                              sceneWidth - TableMargin - CardHeight / 2,
                              sceneHeight / 2, true, -90);
            }
        }
}

void GameTable::drawOtherCardsInPlay(
        const std::vector<ClientPlayerState>& players) {
        constexpr qreal cardSpacing = 90;
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
                CardWidth + (cards.size() > 0 ? (cards.size() - 1) * cardSpacing : 0);

            qreal centerX = 0;
            qreal centerY = 0;
            qreal rotation;
            bool vertical = false;
            if (playerIndex == 0) {
                centerX = sceneWidth / 2;
                centerY = TableMargin + CardHeight * 1.5;
                rotation = 180;
            } else if (playerIndex == 1) {
                centerX = TableMargin + CardHeight * 1.5;
                centerY = sceneHeight / 2;
                rotation = 90;
                vertical = true;
            } else if (playerIndex == 2) {
                centerX = sceneWidth - TableMargin - CardHeight * 1.5;
                centerY = sceneHeight / 2;
                rotation = -90;
                vertical = true;
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
                const qreal offset = cardIndex * cardSpacing - cardsLength / 2 + CardWidth / 2;
                const qreal cardCenterX = vertical ? centerX : centerX + offset;
                const qreal cardCenterY = vertical ? centerY + offset : centerY;
                setRotatedItemGeometry(visualCard, cardCenterX, cardCenterY, rotation);
                scene->addItem(visualCard);
            }
        }
}

void GameTable::drawOtherPlayersDrawAndDiscard(
        const std::vector<ClientPlayerState>& players) {
        const qreal sceneWidth = scene->sceneRect().width();
        const qreal sceneHeight = scene->sceneRect().height();

        const QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem *item : items) {
            if (item->data(0).toString() == "other-player-stack") {
                scene->removeItem(item);
                delete item;
            }
        }

        const auto drawOtherStack = [&](qreal centerX, qreal centerY, qreal rotation,
                                        int count, const Card *topCard) {
            QGraphicsItem *stackItem = nullptr;
            if (topCard && count > 0) {
                VisualCard *card = drawCard(scene, topCard,
                    centerX - CardWidth / 2, centerY - CardHeight / 2,
                    Location::DISCARD_PILE);
                setRotatedItemGeometry(card, centerX, centerY, rotation);
                stackItem = card;
            } else {
                QGraphicsPixmapItem *backCard = drawBackCard(
                    scene, centerX - CardWidth / 2, centerY - CardHeight / 2);
                setRotatedItemGeometry(backCard, centerX, centerY, rotation);
                stackItem = backCard;
            }
            stackItem->setData(0, "other-player-stack");

            QGraphicsTextItem *countText =
                scene->addText(QString::number(count));
            countText->setDefaultTextColor(Qt::white);
            countText->setData(0, "other-player-stack");
            countText->setPos(centerX - countText->boundingRect().width() / 2,
                              centerY + CardHeight / 2 + 2);
        };

        for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex) {
            const ClientPlayerState *player = &players[playerIndex];
            if (!player || playerIndex > 2) {
                continue;
            }

            qreal drawX = 0;
            qreal drawY = 0;
            qreal discardX = 0;
            qreal discardY = 0;
            qreal rotation;
            if (playerIndex == 0) {
                drawX = TableMargin + CardWidth / 2;
                drawY = TableMargin + CardHeight / 2;
                discardX = sceneWidth - TableMargin - CardWidth / 2;
                discardY = TableMargin + CardHeight / 2;
                rotation = 180;
            } else if (playerIndex == 1) {
                drawX = TableMargin + CardHeight / 2;
                drawY = TableMargin + CardWidth / 2;
                discardX = TableMargin + CardHeight / 2;
                discardY = sceneHeight - TableMargin - CardWidth / 2;
                rotation = 90;
            } else {
                drawX = sceneWidth - TableMargin - CardHeight / 2;
                drawY = TableMargin + CardWidth / 2;
                discardX = sceneWidth - TableMargin - CardHeight / 2;
                discardY = sceneHeight - TableMargin - CardWidth / 2;
                rotation = -90;
            }

            drawOtherStack(drawX, drawY, rotation, player->drawStackSize, nullptr);
            drawOtherStack(discardX, discardY, rotation, player->discardStackSize,
                           player->topDiscardCard);
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

void GameTable::drawShop(qreal x, qreal y) {
    const qreal shopWidth = gameState.shop.empty() ? 0 : CardWidth + (gameState.shop.size() - 1) * CardSpacing;
    const qreal shopHeight = CardHeight * 2 + CardSpacing;

    int index = 0;
    for (auto pair : gameState.shop) {
        std::cout << "Drawing shop card: " << pair.first->getName() << " with quantity: " << pair.second << std::endl;
        const Card* card = pair.first;
        const qreal offsetX = index * CardSpacing - shopWidth / 2 + CardWidth / 2;
        const qreal offsetY = (index % 2 == 0) ? 0 : CardHeight + CardSpacing;
        drawCard(scene, card, x + offsetX, y + offsetY)->setData(0, "shop-card");
        QGraphicsTextItem* quantityText = scene->addText(QString::number(pair.second));
        quantityText->setDefaultTextColor(Qt::white);
        quantityText->setPos(x + offsetX, y + offsetY + CardHeight + 2);
        quantityText->setData(0, "shop-card");
        index++;
    }
}

void GameTable::setPhaseText(const QString& phase) {
    emit phaseChanged(phase);
}

void GameTable::displayMessage(QString message){
    QGraphicsTextItem* messageText = scene->addText(message);
    messageText->setDefaultTextColor(Qt::white);
    messageText->setPos(scene->sceneRect().width() / 2 - messageText->boundingRect().width() / 2, scene->sceneRect().height() / 4 - messageText->boundingRect().height() / 2);
}

GameTable::~GameTable() {
    delete ui;
}