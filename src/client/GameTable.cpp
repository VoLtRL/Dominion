#include "client/GameTable.hpp"
#include "client/utils/draw.hpp"
#include "shared/gameplay/Factory.hpp"

#include "ui_GameTable.h"

#include <utility>

#include <QResizeEvent>
#include <QGraphicsTextItem>
#include <QTimer>
#include <algorithm>
#include <QDebug>

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

    messageTimer = new QTimer(this);
    messageTimer->setSingleShot(true);
    connect(messageTimer, &QTimer::timeout, [this]() {
        if(messageTextItem) {
            scene->removeItem(messageTextItem);
            delete messageTextItem;
            messageTextItem = nullptr;
        }
        messageText.clear();
    });

    connect(ui->endTurnButton, &QPushButton::clicked, this, &GameTable::onEndTurnClicked);
    connect(ui->PlayCardButton, &QPushButton::clicked, this, &GameTable::onPlayCardClicked);
    connect(ui->nextPhaseButton, &QPushButton::clicked, this, &GameTable::onNextPhaseClicked);
    connect(ui->buyCardButton, &QPushButton::clicked, this, &GameTable::onBuyCardClicked);

    connect(ui->playReactionButton, &QPushButton::clicked, this, &GameTable::onPlayReactionClicked);

    connect(ui->passReactionButton, &QPushButton::clicked, this, &GameTable::onPassReactionClicked);

    connect(ui->sendSelectionButton, &QPushButton::clicked, this, &GameTable::onSendSelectionClicked);

    connect(ui->playAllTreasuresButton, &QPushButton::clicked, [this]() {
        if (!gameState.isLocalPlayerActive()) {
            displayMessage("It's not your turn.");
            return;
        }

        emit allTreasuresPlayed();
    });

    connect(ui->switchShopButton, &QPushButton::clicked, this, &GameTable::switchShop);

}

QTextEdit* GameTable::getPlayerLogs() const {
    return ui->logs;
}

void GameTable::updateTurnInfos() {
    QTextEdit *playerInfos = ui->turnInfos;
    playerInfos->clear();
    playerInfos->setTextColor(Qt::white);
    playerInfos->append(QString("You: %1").arg(gameState.getYourself()
        ? gameState.getYourself()->name
        : "Unknown"));
    playerInfos->append(QString("Phase: %1").arg(toString(gameState.currentPhase).c_str()));
    playerInfos->append(QString("Player: %1").arg(gameState.activePlayerName));
    playerInfos->append(QString("Actions: %1").arg(gameState.currentPlayerActions));
    playerInfos->append(QString("Purchases: %1").arg(gameState.currentPlayerPurchasesLeft));
    playerInfos->append(QString("Money: %1").arg(gameState.currentPlayerMoney));
}

void GameTable::addLogMessage(const QString &playerName, const Qt::GlobalColor &color, const QString &action) {
    QString logMessage = QString("%1: <span style=\"color: %2;\">%3</span>").arg(playerName, QColor(color).name(), action);
    ui->logs->append(logMessage);
}

QString GameTable::getActivePlayerName(){
    return gameState.activePlayerName;
}

void GameTable::mousePressEvent(QMouseEvent *event) {
    if (zoomedInCardItem) {
        clearZoomedInCard();
    }
    QWidget::mousePressEvent(event);
}


void GameTable::onPlayCardClicked() {
    if (reactionPending) {
        displayMessage("Answer the pending reaction first.");
        return;
    }
    const Card* cardToPlay = nullptr;
    for (VisualCard* visualCard : visualCardsInHand) {
        if (visualCard->isSelected()) {
            cardToPlay = visualCard->getAssociatedCard();
        }
    }

    if (cardToPlay == nullptr) {
        displayMessage("No card selected.");
        return;
    }
    qDebug() << "Attempting to play card: " << cardToPlay->getName().c_str();
    tryPlayCard(cardToPlay);
}

void GameTable::tryPlayCard(const Card* card) {
    if (card == nullptr) {
        displayMessage("No card selected.");
        return;
    }

    if (cardPlayPending) {
        displayMessage("Waiting for the server to confirm the previous card.");
        return;
    }

    if (!gameState.canPlayCard(card)) {
        displayMessage(QString("Cannot play card: %1").arg(QString::fromStdString(card->getName())));
        return;
    }

    cardPlayPending = true;
    emit cardPlayed(card);
}

void GameTable::handleCardPlayed(const Card* card) {
    if (card == nullptr) {
        displayMessage("No card played.");
        return;
    }

    cardPlayPending = false;
}

void GameTable::handleCardBought(const Card* card) {
    if (card == nullptr) {
        displayMessage("No card bought.");
        return;
    }

    if (gameState.isLocalPlayerActive()) {
        displayMessage(QString("You bought: %1, %2").arg(QString::fromStdString(card->getName())).arg(QString::fromStdString(card->getDescription())));
    } else {
        displayMessage(QString("%1 bought: %2").arg(gameState.activePlayerName, QString::fromStdString(card->getName())));
    }
}

void GameTable::handleGameEnded(const QString &winnerName) {
    cardPlayPending = false;
    selectionPending = false;
    reactionPending = false;
    reactionResponsePending = false;
    modifySelection(GamePhase::UNKNOWN, 0, 0, 0);
    displayMessage(QString("Game over. Winner: %1").arg(winnerName));
    addLogMessage("Game", Qt::yellow, QString("Game over. Winner: %1").arg(winnerName));

}

void GameTable::handleSelectionRequest(SelectionMode mode, SelectionConstraint constraint,
                                       int numberOfCards,
                                       const std::vector<const Card*> &cards) {
    selectionPending = true;
    setSelection(mode, constraint, numberOfCards, cards);
    displayMessage("Select the requested cards in the center, then send your selection.");
    if (!isShopVisible && mode == SelectionMode::GAIN) {
        switchShop();
    }
    modifySelection(gameState.currentPhase, gameState.currentPlayerActions,
                    gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);
}

void GameTable::handleCardPlayRejected(const QString &message) {
    cardPlayPending = false;
    selectionPending = false;
    reactionResponsePending = false;
    modifySelection(gameState.currentPhase, gameState.currentPlayerActions,
                    gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);
    displayMessage(message);
}

void GameTable::onNextPhaseClicked() {
    if (!gameState.isLocalPlayerActive()) {
        displayMessage("It's not your turn.");
        return;
    }
    emit nextPhaseClicked();
}

void GameTable::startTurn() {
    setPhaseText(gameState.currentPhase);
    if(gameState.isLocalPlayerActive()) {
        displayMessage("It's your turn");
        modifySelection(gameState.currentPhase, gameState.currentPlayerActions, gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);
    } else {
        displayMessage(QString("%1's turn").arg(gameState.activePlayerName));
    }

    switchShop();
}

void GameTable::updatePhase(GamePhase newPhase) {
    changePhase(newPhase);
    auto phaseName = QString::fromStdString(toString(newPhase));
    if(gameState.isLocalPlayerActive()) {
        displayMessage(QString("Phase changed to: %1").arg(QString::fromStdString(toString(newPhase))));
        addLogMessage("You", Qt::cyan, QString("moved to phase: %1").arg(phaseName));
    } else {
        displayMessage(QString("%1's phase changed to: %2").arg(gameState.activePlayerName, QString::fromStdString(toString(newPhase))));
        addLogMessage(gameState.activePlayerName, Qt::cyan, QString("moved to phase: %1").arg(phaseName));
    }
}

void GameTable::changePhase(GamePhase newPhase) {
    gameState.currentPhase = newPhase;
    
    modifySelection(gameState.currentPhase, gameState.currentPlayerActions, gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);
}

void GameTable::handleReactionRequest(const std::vector<const Card*> &reactions) {
    reactionPending = true;
    reactionResponsePending = false;
    setSelection(SelectionMode::REACTION, SelectionConstraint::AT_MOST, 1, reactions);
    for (VisualCard *card : visualCardsInHand) {
        card->setSelected(false);
    }
    if (reactions.empty()) {
        ui->playReactionButton->setToolTip("No reaction cards available to play. You must pass.");
    } else {
        ui->playReactionButton->setToolTip("Play the selected reaction card.");
        ui->playReactionButton->setEnabled(true);
    }
    ui->passReactionButton->setEnabled(true);
    ui->passReactionButton->setToolTip("Pass the reaction.");
    modifySelection(gameState.currentPhase, gameState.currentPlayerActions,
                    gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);
    displayMessage("Choose a reaction or pass.");
}

void GameTable::handleReactionCompleted() {
    qDebug() << "Reaction completed.";
    reactionPending = false;
    reactionResponsePending = false;
    setSelection(SelectionMode::NONE, SelectionConstraint::NONE, 0);
    modifySelection(gameState.currentPhase, gameState.currentPlayerActions,
                    gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);
    updateSelectionVisibility();
}

void GameTable::onPlayReactionClicked() {
    if (reactionResponsePending) {
        return;
    }
    const Card* reactionCard = nullptr;
    for (VisualCard* visualCard : visualCardsToSelect) {
        if (visualCard->isSelected()) {
            reactionCard = visualCard->getAssociatedCard();
        }
    }

    if (!reactionCard) {
        displayMessage("No reaction card selected.");
        return;
    }

    const bool isAvailable = std::any_of(
        gameState.selection.availableCards.begin(),
        gameState.selection.availableCards.end(),
        [reactionCard](const Card *card) {
            return card && card->getName() == reactionCard->getName();
        });
    if(!reactionPending || !isAvailable || !reactionCard->isA(CardType::REACTION)) {
        displayMessage(QString("Cannot play reaction card: %1").arg(QString::fromStdString(reactionCard->getName())));
        return;
    }

    reactionResponsePending = true;
    ui->playReactionButton->setEnabled(false);
    ui->passReactionButton->setEnabled(false);
    emit reactionPlayed(reactionCard);
}

void GameTable::onPassReactionClicked() {
    if(!reactionPending || reactionResponsePending) {
        displayMessage("Cannot pass reaction.");
        return;
    }
    
    reactionResponsePending = true;
    ui->playReactionButton->setEnabled(false);
    ui->passReactionButton->setEnabled(false);
    emit reactionPassed();
}

void GameTable::switchShop() {
    const bool showShop = !isShopVisible;
    if (isShopVisible) {
        const QList<QGraphicsItem*> items = scene->items();
        for (QGraphicsItem *item : items) {
            if (item->data(0).toString() == "shop-card") {
                scene->removeItem(item);
                delete item;
            }
        }
    }
    else {
        drawShop();
    }

    isShopVisible = showShop;
    updateSelectionVisibility();
}


void GameTable::onBuyCardClicked() {
    if (reactionPending) {
        displayMessage("Answer the pending reaction first.");
        return;
    }
    const Card* cardToBuy = nullptr;
    for (VisualCard* visualCard : visualCardsInShop) {
        if (visualCard->isSelected()) {
            cardToBuy = visualCard->getAssociatedCard();
        }
    }

    if (!cardToBuy) {
        displayMessage("No card selected.");
        return;
    }

    if (!gameState.canBuyCard(cardToBuy)) {
        displayMessage(QString("Cannot buy card: %1").arg(QString::fromStdString(cardToBuy->getName())));
        return;
    }

    emit cardBought(cardToBuy);
}

/// @brief Modifies the selection state of the game table based on the current game phase, actions, purchases, and money.
/// @param phase The current game phase (e.g., ACTION, BUY, UNKNOWN).
/// @param actions The number of actions the player has left.
/// @param purchases The number of purchases the player has left.
/// @param money The amount of money the player has available for purchases.
void GameTable::modifySelection(GamePhase phase, int actions, int purchases, int money) {
    gameState.currentPhase = phase;
    gameState.currentPlayerActions = actions;
    gameState.currentPlayerPurchasesLeft = purchases;
    gameState.currentPlayerMoney = money;

    const UiPermissions permissions = computePermissions(gameState);
    ui->PlayCardButton->setVisible(permissions.showPlayCard);
    ui->PlayCardButton->setEnabled(permissions.canPlayCard);
    ui->playAllTreasuresButton->setVisible(permissions.showPlayAllTreasures);
    ui->playAllTreasuresButton->setEnabled(permissions.canPlayAllTreasures);
    ui->buyCardButton->setVisible(permissions.showBuyCard);
    ui->buyCardButton->setEnabled(permissions.canBuyCard);
    ui->nextPhaseButton->setVisible(permissions.showNextPhase);
    ui->nextPhaseButton->setEnabled(permissions.canAdvancePhase);
    ui->endTurnButton->setVisible(permissions.showEndTurn);
    ui->endTurnButton->setEnabled(permissions.canEndTurn);
    ui->sendSelectionButton->setVisible(permissions.showSendSelection);
    ui->sendSelectionButton->setEnabled(permissions.canSendSelection);
    ui->playReactionButton->setVisible(permissions.showPlayReaction);
    ui->playReactionButton->setEnabled(permissions.canPlayReaction);
    ui->passReactionButton->setVisible(permissions.showPassReaction);
    ui->passReactionButton->setEnabled(permissions.canPassReaction);
    ui->switchShopButton->setVisible(permissions.showSwitchShop);


    if ((gameState.currentPhase != phase) && phase != GamePhase::UNKNOWN) {
        setPhaseText(phase);
    }
}


/// @brief It computes the UI permissions based on the current client game state. It determines which actions are available to the player,
/// such as playing cards, buying cards, advancing phases, and sending selections. The permissions are computed based on the player's turn, selection state, reaction state, and the current game phase.
/// I hate this function. :(
/// @param state The current client game state.
/// @return The computed UI permissions based on the game state.
UiPermissions GameTable::computePermissions(const ClientGameState& state) const {
    UiPermissions permissions;
    const bool localTurn = state.isLocalPlayerActive();
    const bool selecting = selectionPending && !reactionPending;
    const bool reacting = reactionPending;
    const bool waitingForResponse = reactionResponsePending;
    const bool normalTurn = localTurn && !selecting && !reacting;
    const bool actionPhase = state.currentPhase == GamePhase::ACTION;
    const bool buyPhase = state.currentPhase == GamePhase::BUY;
    const bool hasReactionCard = !state.selection.availableCards.empty();

    permissions.showPlayCard = normalTurn && (actionPhase || buyPhase);
    permissions.canPlayCard = normalTurn && ((actionPhase && state.currentPlayerActions > 0) || buyPhase);
    permissions.showPlayAllTreasures = normalTurn && (actionPhase || buyPhase);
    permissions.canPlayAllTreasures = permissions.showPlayAllTreasures;
    permissions.showBuyCard = normalTurn && buyPhase;
    permissions.canBuyCard = permissions.showBuyCard && state.currentPlayerPurchasesLeft > 0;
    permissions.showNextPhase = normalTurn && actionPhase;
    permissions.canAdvancePhase = permissions.showNextPhase;
    permissions.showEndTurn = normalTurn && buyPhase;
    permissions.canEndTurn = permissions.showEndTurn;
    permissions.showSendSelection = selecting;
    permissions.canSendSelection = selecting;
    permissions.showPlayReaction = reacting && !waitingForResponse && hasReactionCard;
    permissions.canPlayReaction = permissions.showPlayReaction;
    permissions.showPassReaction = reacting;
    permissions.canPassReaction = reacting && !waitingForResponse;
    permissions.showSwitchShop = !selecting && !reacting;
    return permissions;
}

/// @brief Sets the selection state of the game table, including the selection mode, constraint, number of cards to select, and available cards for selection.
/// @param mode The selection mode (e.g., NONE, GAIN, REACTION).
/// @param constraint The selection constraint (e.g., NONE, AT_MOST, EXACTLY).
/// @param numberOfCards The number of cards to select (must be non-negative).
/// @param availableCards The list of available cards for selection (default is an empty vector).
void GameTable::setSelection(SelectionMode mode,
                             SelectionConstraint constraint,
                             int numberOfCards,
                             const std::vector<const Card*> &availableCards) {
    gameState.selection.mode = mode;
    gameState.selection.constraint = constraint;
    gameState.selection.numberOfCards = std::max(0, numberOfCards);
    gameState.selection.availableCards = availableCards;
    gameState.selection.isPending = mode != SelectionMode::NONE;

    // The idea is to store the available cards into the visualCardsToSelect vector, which will be used to determine which cards can be selected by the player.
    // It will be used to display the available cards in the UI and to check if the player's selection is valid.
    drawCardsToSelect();
    updateSelectionVisibility();
}

void GameTable::drawCardsToSelect() {
    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    visualCardsToSelect.clear();

    for (QGraphicsItem *item : scene->items()) {
        if (item->data(0).toString() == "selection-card") {
            scene->removeItem(item);
            delete item;
        }
    }

    const size_t numCards = gameState.selection.availableCards.size();
    if (numCards == 0) return;

    const int columns = (numCards + 1) / 2;
    const int rows = std::min<int>(numCards, 2);

    const qreal spacingX = 110.0;
    const qreal spacingY = CardHeight + 20.0;

    const qreal totalWidth = CardWidth + (columns - 1) * spacingX;
    const qreal totalHeight = CardHeight + (rows - 1) * spacingY;

    const qreal startX = (sceneWidth - totalWidth) / 2.0;
    const qreal startY = (sceneHeight - totalHeight) / 2.0;

    for (size_t index = 0; index < numCards; ++index) {
        const Card *card = gameState.selection.availableCards[index];
        if (!card) continue;

        auto visualCard = drawCard(scene, card, startX + index / 2 * spacingX, startY + index % 2 * spacingY, Location::IN_SELECTION);

        connect(visualCard, &VisualCard::clicked, [this, visualCard]() {
            if (visualCard->isSelected()) {
                visualCard->setSelected(false);
            } else {
                int selectedCount = 0;
                for (VisualCard *vCard : visualCardsToSelect) {
                    if (vCard->isSelected()) {
                        ++selectedCount;
                    }
                }
                if (gameState.selection.constraint != SelectionConstraint::NONE &&
                    selectedCount >= gameState.selection.numberOfCards) {
                    displayMessage(QString("You need to select %1 %2 card(s).")
                                    .arg(toString(gameState.selection.constraint).c_str())
                                    .arg(gameState.selection.numberOfCards));
                    return;
                }
                visualCard->setSelected(true);
            }
        });
        connect(visualCard, &VisualCard::rightClicked, visualCard, [this, visualCard](int cardId) {
            cardBeingRightClicked = visualCard->getAssociatedCard();
            drawZoomedInCard();
        });

        visualCard->setZValue(2);
        visualCard->setData(0, "selection-card");
        visualCardsToSelect.push_back(visualCard);
    }
}

bool GameTable::isLocalPlayerActive() const {
    return gameState.isLocalPlayerActive();
}


/// @brief Updates the game state of the GameTable with a new ClientGameState. It checks for changes in the player's hand, cards in play, shop, and other players' states, and updates the UI.
/// I love this function.
/// @param newGameState 
void GameTable::updateGameState(ClientGameState newGameState) {
    const bool isFirstLoad = gameState.players.empty();
    const ClientPlayerState* oldYourself = gameState.getYourself();
    const ClientPlayerState* newYourself = newGameState.getYourself();

    const bool handChanged = !oldYourself || !newYourself || (gameState.hand.getCards() != newGameState.hand.getCards());
    const bool playChanged = (gameState.cardsInPlay != newGameState.cardsInPlay);
    const bool shopChanged = (gameState.shop != newGameState.shop);
    const bool playersChanged = (gameState.players != newGameState.players);
    const bool stacksChanged = !oldYourself || !newYourself || 
                               (oldYourself->drawStackSize != newYourself->drawStackSize) ||
                               (oldYourself->discardStackSize != newYourself->discardStackSize) ||
                               (oldYourself->topDiscardCard != newYourself->topDiscardCard);

    const ClientSelectionState previousSelection = gameState.selection;
    
    gameState = newGameState;
    
    if (newGameState.currentPendingInteractionPhase == PendingInteractionPhase::NONE) {
        gameState.selection = ClientSelectionState();
    } 
    else {
        gameState.selection = previousSelection;
    }

    const bool selectionChanged = (previousSelection != gameState.selection);

    updateTurnInfos();
    modifySelection(gameState.currentPhase, gameState.currentPlayerActions,
                    gameState.currentPlayerPurchasesLeft, gameState.currentPlayerMoney);

    if (isFirstLoad || !newYourself) {
        drawTable();
        return;
    }

    if (handChanged)    drawPlayerHand(gameState.hand.getCards());
    if (stacksChanged)  drawStacks(newYourself->drawStackSize, newYourself->discardStackSize, newYourself->topDiscardCard);
    if (playChanged)    drawCardsInPlay(gameState.cardsInPlay);
    if (playersChanged) {
        std::vector<ClientPlayerState> opponents;
        for (const ClientPlayerState &player : gameState.players) {
            if (!player.isYourself) {
                opponents.push_back(player);
            }
        }
        drawOtherPlayers(opponents);
    }
    if (shopChanged && isShopVisible) drawShop();
    if (selectionChanged) drawCardsToSelect();
    
    updateSelectionVisibility();
}

void GameTable::drawTable() {
    // Clear the scene
    const bool shouldRestoreMessage =
        messageTextItem != nullptr && messageTimer->isActive() && !messageText.isEmpty();
    const QString messageToRestore = messageText;

    const QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem *item : items) {
        scene->removeItem(item);
        delete item;
    }

    // clear every containers
    messageTextItem = nullptr;
    zoomedInCardItem = nullptr;
    visualCardsInHand.clear();
    visualCardsInPlay.clear();
    visualCardsInShop.clear();
    otherPlayersHandItems.clear();
    otherPlayersInPlayItems.clear();
    otherPlayersStackItems.clear();

    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    const qreal handWidth = gameState.hand.getCards().empty() ? 0 : CardWidth + (gameState.hand.getCards().size() - 1) * CardSpacing;
    const ClientPlayerState* yourself = gameState.getYourself();
    if (!yourself) {
        displayMessage("Local player state is unavailable.");
        return;
    }

    std::vector<ClientPlayerState> opponents;
    for (const ClientPlayerState &player : gameState.players) {
        std::cout << "Player: " << player.name.toStdString() << ", is yourself: " << player.isYourself << std::endl;
        if (!player.isYourself) {
            opponents.push_back(player);
        }
    }

    updateTurnInfos();

    // Draw the table
    drawPlayerHand(gameState.hand.getCards());
    drawStacks(yourself->drawStackSize, yourself->discardStackSize, yourself->topDiscardCard);
    const qreal cardsInPlayWidth = gameState.cardsInPlay.empty() ? 0 : CardWidth + (gameState.cardsInPlay.size() - 1) * 90;
    drawCardsInPlay(gameState.cardsInPlay);
    drawCardsToSelect();
    drawOtherPlayers(opponents);


    if(isShopVisible) drawShop();

    drawZoomedInCard();

    

    if (shouldRestoreMessage) {
        messageTextItem = scene->addText(messageToRestore);
        messageTextItem->setDefaultTextColor(Qt::white);
        messageTextItem->setPos(
            (sceneWidth - messageTextItem->boundingRect().width()) / 2,
            sceneHeight / 4 - messageTextItem->boundingRect().height() / 2);
    }
    updateSelectionVisibility();
}

void GameTable::onEndTurnClicked() {
    emit endTurnClicked();
}

void GameTable::onSendSelectionClicked() {
    if (!selectionPending) {
        displayMessage("No selection is pending.");
        return;
    }

    std::vector<const Card*> selectedCards;
    for (VisualCard *visualCard : visualCardsToSelect) {
        if (visualCard->isSelected()) {
            selectedCards.push_back(visualCard->getAssociatedCard());
        }
    }

    bool isValidSelection = gameState.isSelectionValid(selectedCards.size());

    if (!isValidSelection) {
        displayMessage(QString("You must select %1 %2 card(s).").arg(toString(gameState.selection.constraint).c_str()).arg(gameState.selection.numberOfCards));
        return;
    }
    selectionPending = false;
    updateSelectionVisibility();

    emit selectionSubmitted(selectedCards);
}

void GameTable::drawZoomedInCard() {
    if (zoomedInCardItem) {
        scene->removeItem(zoomedInCardItem);
        delete zoomedInCardItem;
        zoomedInCardItem = nullptr;
    }
    if (cardBeingRightClicked) {
        auto vCard = drawCard(scene, cardBeingRightClicked, 0, 0);
        zoomedInCardItem = vCard;
        zoomedInCardItem->setZValue(10);
        zoomedInCardItem->setScale(4.0);
        zoomedInCardItem->setPos((scene->sceneRect().width() - CardWidth * 4.0) / 2, (scene->sceneRect().height() - CardHeight * 4.0) / 2);
    }
}

void GameTable::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    scene->setSceneRect(ui->gameView->viewport()->rect());
    drawTable();
    emit widgetResized(event->size());
}

void GameTable::drawPlayerHand(const std::vector<const Card*> hand) {

    qreal sceneWidth = scene->sceneRect().width();
    qreal sceneHeight = scene->sceneRect().height();

    qreal handWidth = hand.empty() ? 0 : CardWidth + (hand.size() - 1) * CardSpacing;

    qreal x = (sceneWidth - handWidth) / 2;
    qreal y = sceneHeight - CardHeight - TableMargin;

    visualCardsInHand.clear();
    std::vector<VisualCard*> visualCards = ::drawPlayerHand(scene, hand, x, y);
    for (VisualCard* visualCard : visualCards) {
        visualCardsInHand.push_back(visualCard);
        connect(visualCard, &VisualCard::clicked, visualCard, [this, visualCard](int cardId) {
            if (selectionPending || reactionPending) {
                displayMessage("Select a card from the cards shown in the center.");
                return;
            }
            int selectedCount = 0;
            for (VisualCard *card : visualCardsInHand) {
                if (card && card->isSelected()) {
                    selectedCount++;
                }
                updateSelectionVisibility();
            }
            if (!(selectionPending || reactionPending) && !visualCard->isSelected() &&
                !gameState.canSelectAnotherCard(selectedCount)) {
                displayMessage("Only one card can be selected.");
                return;
            }

            visualCard->setSelected(!visualCard->isSelected());
        });
        connect(visualCard, &VisualCard::rightClicked, visualCard, [this, visualCard](int cardId) {
            cardBeingRightClicked = visualCard->getAssociatedCard();
            drawZoomedInCard();
        });
    }
}

void GameTable::drawOtherPlayersHands(const std::vector<ClientPlayerState>& players) {
    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    for (QGraphicsItem *item : otherPlayersHandItems) {
        scene->removeItem(item);
        delete item;
    }
    otherPlayersHandItems.clear();

    const auto drawOtherHand = [&](int numberOfCards, qreal centerX, qreal centerY, bool vertical, qreal rotation) {
        if (numberOfCards <= 0) return;

        const qreal handLength = CardWidth + (numberOfCards - 1) * CardSpacing;
        for (size_t cardIndex = 0; cardIndex < numberOfCards; ++cardIndex) {
            const qreal offset = cardIndex * CardSpacing - handLength / 2 + CardWidth / 2;
            const qreal cardCenterX = vertical ? centerX : centerX + offset;
            const qreal cardCenterY = vertical ? centerY + offset : centerY;
            QGraphicsPixmapItem *backCard = drawBackCard(
                scene, cardCenterX - CardWidth / 2, cardCenterY - CardHeight / 2);
            backCard->setData(0, "other-player-hand");
            setRotatedItemGeometry(backCard, cardCenterX, cardCenterY, rotation);
            
            otherPlayersHandItems.push_back(backCard);
        }
    };

    for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex) {
        const ClientPlayerState *player = &players[playerIndex];
        if (!player) continue;

        if (playerIndex == 0) {
            drawOtherHand(player->handSize, sceneWidth / 2, TableMargin + CardHeight / 2, false, 180);
        } else if (playerIndex == 1) {
            drawOtherHand(player->handSize, TableMargin + CardHeight / 2, sceneHeight / 2, true, 90);
        } else if (playerIndex == 2) {
            drawOtherHand(player->handSize, sceneWidth - TableMargin - CardHeight / 2, sceneHeight / 2, true, -90);
        }
    }
}

void GameTable::drawOtherCardsInPlay(const std::vector<ClientPlayerState>& players) {
    constexpr qreal cardSpacing = 90;
    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    for (VisualCard *visualCard : otherPlayersInPlayItems) {
        scene->removeItem(visualCard);
        delete visualCard;
    }
    otherPlayersInPlayItems.clear();

    for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex) {
        const auto &cards = players[playerIndex].cardsInPlay;
        const qreal cardsLength = CardWidth + (cards.size() > 0 ? (cards.size() - 1) * cardSpacing : 0);

        qreal centerX = 0; qreal centerY = 0; qreal rotation = 0; bool vertical = false;
        
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
        } else continue;

        for (size_t cardIndex = 0; cardIndex < cards.size(); ++cardIndex) {
            const Card *card = cards[cardIndex];
            VisualCard *visualCard = new VisualCard(
                VisualCard::getNewId(), card, QString::fromStdString(card->getName()),
                card->getCost(), QString(card->getPath().c_str()), Location::OTHER_PLAYER_IN_PLAY);
                
            connect(visualCard, &VisualCard::rightClicked, visualCard, [this, visualCard](int cardId) {
                cardBeingRightClicked = visualCard->getAssociatedCard();
                drawZoomedInCard();
            });
            visualCard->setData(0, "other-player-in-play");
            const qreal offset = cardIndex * cardSpacing - cardsLength / 2 + CardWidth / 2;
            const qreal cardCenterX = vertical ? centerX : centerX + offset;
            const qreal cardCenterY = vertical ? centerY + offset : centerY;
            setRotatedItemGeometry(visualCard, cardCenterX, cardCenterY, rotation);
            scene->addItem(visualCard);
            
            otherPlayersInPlayItems.push_back(visualCard);
        }
    }
}

void GameTable::drawOtherPlayersDrawAndDiscard(const std::vector<ClientPlayerState>& players) {
    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    for (QGraphicsItem *item : otherPlayersStackItems) {
        scene->removeItem(item);
        delete item;
    }
    otherPlayersStackItems.clear();

    const auto drawOtherStack = [&](qreal centerX, qreal centerY, qreal rotation, int count, const Card *topCard) {
        QGraphicsItem *stackItem = nullptr;
        if (topCard && count > 0) {
            VisualCard *card = drawCard(scene, topCard, centerX - CardWidth / 2, centerY - CardHeight / 2, Location::DISCARD_PILE);
            setRotatedItemGeometry(card, centerX, centerY, rotation);
            stackItem = card;
        } else {
            QGraphicsPixmapItem *backCard = drawBackCard(scene, centerX - CardWidth / 2, centerY - CardHeight / 2);
            setRotatedItemGeometry(backCard, centerX, centerY, rotation);
            stackItem = backCard;
        }
        stackItem->setData(0, "other-player-stack");
        otherPlayersStackItems.push_back(stackItem);

        QGraphicsTextItem *countText = scene->addText(QString::number(count));
        countText->setDefaultTextColor(Qt::white);
        countText->setData(0, "other-player-stack");
        countText->setPos(centerX - countText->boundingRect().width() / 2, centerY + CardHeight / 2 + 2);
        otherPlayersStackItems.push_back(countText);
    };

    for (size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex) {
        const ClientPlayerState *player = &players[playerIndex];
        if (!player || playerIndex > 2) continue;

        qreal drawX = 0; qreal drawY = 0; qreal discardX = 0; qreal discardY = 0; qreal rotation = 0;
        
        if (playerIndex == 0) {
            drawX = TableMargin + CardWidth / 2 + sceneWidth / 6;
            drawY = TableMargin + CardHeight / 2;
            discardX = sceneWidth - TableMargin - CardWidth / 2 - sceneWidth / 6;
            discardY = TableMargin + CardHeight / 2;
            rotation = 180;
        } else if (playerIndex == 1) {
            drawX = TableMargin + CardHeight / 2;
            drawY = TableMargin + CardWidth / 2 + sceneHeight / 21;
            discardX = TableMargin + CardHeight / 2;
            discardY = sceneHeight - TableMargin - CardWidth / 2 - sceneHeight / 12;
            rotation = 90;
        } else {
            drawX = sceneWidth - TableMargin - CardHeight / 2;
            drawY = TableMargin + CardWidth / 2 + sceneHeight / 21;
            discardX = sceneWidth - TableMargin - CardHeight / 2;
            discardY = sceneHeight - TableMargin - CardWidth / 2 - sceneHeight / 21;
            rotation = -90;
        }

        drawOtherStack(drawX, drawY, rotation, player->drawStackSize, nullptr);
        drawOtherStack(discardX, discardY, rotation, player->discardStackSize, player->topDiscardCard);
    }
}

void GameTable::drawCardsInPlay(const std::vector<const Card*> cardsInPlay) {
    qreal sceneWidth = scene->sceneRect().width();
    qreal sceneHeight = scene->sceneRect().height();
    qreal cardsInPlayWidth = cardsInPlay.empty() ? 0 : CardWidth + (cardsInPlay.size() - 1) * CardSpacing;
    qreal x = (sceneWidth - cardsInPlayWidth) / 2;
    qreal y = sceneHeight - TableMargin - CardHeight*2.5;
    visualCardsInPlay.clear();
    const QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem *item : items) {
        VisualCard *visualCard = dynamic_cast<VisualCard*>(item);
        if (visualCard && visualCard->data(0).toString() == "in-play-card") {
            scene->removeItem(visualCard);
            delete visualCard;
        }
    }

    for (const Card* card : cardsInPlay) {
        VisualCard *visualCard = drawCard(scene, card, x, y);
        connect(visualCard, &VisualCard::rightClicked, visualCard, [this, visualCard](int cardId) {
            cardBeingRightClicked = visualCard->getAssociatedCard();
            drawZoomedInCard();
        });
        visualCardsInPlay.push_back(visualCard);
        visualCardsInPlay.back()->setData(0, "in-play-card"); // this is a card in play.
        x += 90; // Move to the right for the next card
    }
}

void GameTable::clearCardsInPlay() {
    for (VisualCard *visualCard : visualCardsInPlay) {
        scene->removeItem(visualCard);
        delete visualCard;
    }
    visualCardsInPlay.clear();
    
    for (VisualCard *visualCard : otherPlayersInPlayItems) {
        scene->removeItem(visualCard);
        delete visualCard;
    }
    otherPlayersInPlayItems.clear();
}

void GameTable::drawStacks(quint16 cardsInDraw, quint16 cardsInDiscard, const Card* topDiscardCard) {
    constexpr qreal cardWidth = 100;
    constexpr qreal cardHeight = 150;
    const QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem *item : items) {
        if (item->data(0).toString() == "local-stack") {
            scene->removeItem(item);
            delete item;
        }
    }

    const qreal sceneWidth = scene->sceneRect().width();
    const qreal sceneHeight = scene->sceneRect().height();

    const qreal drawX = TableMargin + sceneWidth / 6; 
    const qreal discardX = sceneWidth - TableMargin - sceneWidth / 6 - cardWidth; 
    const qreal stackY = sceneHeight - cardHeight - 20;

    QGraphicsPixmapItem *drawCardItem = drawBackCard(scene, drawX, stackY);
    drawCardItem->setData(0, "local-stack");
    if (topDiscardCard) {
        VisualCard* visualCard = drawCard(scene, topDiscardCard, discardX, stackY);
        visualCard->setData(0, "local-stack");
        connect(visualCard, &VisualCard::rightClicked, visualCard, [this, visualCard](int cardId) {
            cardBeingRightClicked = visualCard->getAssociatedCard();
            drawZoomedInCard();
        });
    } else {
        QGraphicsPixmapItem *discardCardItem = drawBackCard(scene, discardX, stackY);
        discardCardItem->setData(0, "local-stack");
    }

    QGraphicsTextItem* drawText = scene->addText(QString::number(cardsInDraw));
    drawText->setData(0, "local-stack");
    drawText->setPos(drawX, stackY + cardHeight + 2);
    QGraphicsTextItem* discardText = scene->addText(QString::number(cardsInDiscard));
    discardText->setData(0, "local-stack");
    discardText->setPos(discardX, stackY + cardHeight + 2);
    drawText->setDefaultTextColor(Qt::white);
    discardText->setDefaultTextColor(Qt::white);
}

void GameTable::drawShop() {
    const QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem *item : items) {
        if (item->data(0).toString() == "shop-card") {
            scene->removeItem(item);
            delete item;
        }
    }
    visualCardsInShop.clear();

    const qreal shopWidth = ((gameState.shop.size() + 1) / 2) == 0 ? 0 : (CardWidth + (((gameState.shop.size() + 1) / 2) - 1) * CardSpacing);
    const qreal shopHeight = CardHeight * 2 + CardSpacing;

    qreal startX = (scene->sceneRect().width() - shopWidth) / 2.0;
    qreal startY = (scene->sceneRect().height() - shopHeight) / 2.0;

    int index = 0;
    std::vector<std::pair<const Card*, int>> sortedShopCards(gameState.shop.begin(), gameState.shop.end());
    std::sort(sortedShopCards.begin(), sortedShopCards.end(), [](const std::pair<const Card*, int>& a, const std::pair<const Card*, int>& b) {
        return a.first->getCost() < b.first->getCost();
    });
    for (auto pair : sortedShopCards) {
        const Card* card = pair.first;
        const qreal offsetX = index / 2 * CardSpacing;
        const qreal offsetY = index % 2 * (CardHeight + CardSpacing);
        VisualCard* visualCard = drawCard(scene, card, startX + offsetX, startY + offsetY);
        visualCard->setZValue(4);
        visualCard->setData(0, "shop-card");
        visualCardsInShop.push_back(visualCard);
        connect(visualCard, &VisualCard::clicked, visualCard, [this, visualCard]() {
            if (selectionPending) {
                bool isAllowed = std::find(gameState.selection.availableCards.begin(),
                                            gameState.selection.availableCards.end(),
                                            visualCard->getAssociatedCard()) != gameState.selection.availableCards.end();
                if (!isAllowed) {
                    displayMessage("This card is not part of the requested selection.");
                    return;
                }
            }

            int selectedCount = 0;
            for (VisualCard *card : visualCardsInShop) {
                if (card && card->isSelected()) {
                    selectedCount++;
                }
            }
            if (!visualCard->isSelected() &&
                selectedCount >= 1) {
                displayMessage("Only one card can be selected.");
                return;
            }
            if (!selectionPending && !(gameState.currentPhase == GamePhase::BUY)) {
                displayMessage("This is not the right time to buy cards.");
                return;
            }

            visualCard->setSelected(!visualCard->isSelected());
            displayMessage(visualCard->isSelected()
                ? QString("Selected: %1").arg(QString::fromStdString(visualCard->getAssociatedCard()->getName()))
                : QString("Unselected: %1").arg(QString::fromStdString(visualCard->getAssociatedCard()->getName())));
        });

        connect(visualCard, &VisualCard::rightClicked, visualCard, [this, visualCard](int cardId) {
            cardBeingRightClicked = visualCard->getAssociatedCard();
            drawZoomedInCard();
        });

        QGraphicsTextItem* quantityText = scene->addText(QString::number(pair.second));
        quantityText->setDefaultTextColor(Qt::white);
        quantityText->setPos(startX + offsetX + (CardWidth / 2) - 10, startY + offsetY + CardHeight + 2);
        quantityText->setData(0, "shop-card");
        index++;
    }
}

void GameTable::updateSelectionVisibility() {
    const bool hideCards = selectionPending || reactionPending;
    for (VisualCard *card : visualCardsInHand) {
        if (card) {
            card->setVisible(!hideCards);
        }
    }
    for (VisualCard *card : visualCardsInShop) {
        if (card) {
            card->setVisible(isShopVisible && !hideCards);
        }
    }
    for (QGraphicsItem *item : scene->items()) {
        if (item->data(0).toString() == "shop-card") {
            item->setVisible(isShopVisible && !hideCards);
        }
    }
}

void GameTable::setPhaseText(GamePhase phase) {
    emit phaseChanged(phase);
}

void GameTable::displayMessage(QString message){
    messageText = message;

    if(messageTextItem){
        scene->removeItem(messageTextItem);
        delete messageTextItem;
    }

    messageTextItem = scene->addText(message);
    messageTextItem->setDefaultTextColor(Qt::white);
    messageTextItem->setPos((scene->sceneRect().width() - messageTextItem->boundingRect().width()) / 2, scene->sceneRect().height()/4 - messageTextItem->boundingRect().height()/2);

    messageTimer->start(4000); // Display the message for 4 seconds
}

void GameTable::clearZoomedInCard() {
    if (zoomedInCardItem) {
        scene->removeItem(zoomedInCardItem);
        delete zoomedInCardItem;
        zoomedInCardItem = nullptr;
    }
    cardBeingRightClicked = nullptr;
}

GameTable::~GameTable() {
    delete ui;
}

void GameTable::drawOtherPlayers(const std::vector<ClientPlayerState>& players) {
    drawOtherPlayersHands(players);
    drawOtherCardsInPlay(players);
    drawOtherPlayersDrawAndDiscard(players);
}