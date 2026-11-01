#pragma once

#include <QWidget>
#include <QGraphicsScene>
#include <QString>

#include "shared/abstract/Card.hpp"
#include "shared/player/Player.hpp"
#include "client/VisualCard.hpp"
#include "client/Game/ClientGameState.hpp"
#include "shared/structs/GamePhase.hpp"


QT_BEGIN_NAMESPACE
namespace Ui { class GameTable; }
QT_END_NAMESPACE

class GameTable : public QWidget {
    Q_OBJECT

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::GameTable *ui;
    QGraphicsScene *scene;

    ClientGameState gameState;

    bool isShopVisible{false};

    QVector<VisualCard*> visualCardsInHand;
    QVector<VisualCard*> visualCardsInPlay;
    QVector<VisualCard*> visualCardsInShop;

    QTimer* messageTimer{nullptr};
    QString messageText;
    QGraphicsTextItem* messageTextItem{nullptr};

    QGraphicsTextItem* playerInfosTextItem{nullptr};
    bool cardPlayPending{false};
    bool selectionPending{false};


signals:
    void endTurnClicked();
    void widgetResized(const QSize &newSize);
    void phaseChanged(GamePhase newPhase);
    void cardPlayed(const Card *card);
    void cardBought(const Card *card);
    void nextPhaseClicked();
    void allTreasuresPlayed();
    void selectionSubmitted(const std::vector<const Card*> &cards);
    
private slots:
    void onEndTurnClicked();
    void onPlayCardClicked();
    void onBuyCardClicked();
    void onNextPhaseClicked();

public:
    explicit GameTable(QWidget *parent = nullptr);
    ~GameTable() override;

    void updateGameState(ClientGameState newGameState);

    // Game Logic
    void startTurn();
    void changePhase(GamePhase newPhase);
    void switchShop();
    void endTurn();
    void tryPlayCard(const Card* card);

    void modifySelection(GamePhase phase, int actions, int purchases, int money);
    void setSelection(SelectionMode mode, SelectionConstraint constraint,
                      int numberOfCards,
                      const std::vector<const Card*> &availableCards = {});
    bool isLocalPlayerActive() const;

    /// @brief Handles the event when a card is played after being validated by the server. This function is called when the server confirms that a card has been played successfully.
    /// @param card The card that was played.
    void handleCardPlayed(const Card* card);
    void handleCardPlayRejected(const QString &message);

    void handleCardBought(const Card* card);
    void handleGameEnded(const QString &winnerName);
    void handleSelectionRequest(SelectionMode mode, SelectionConstraint constraint,
                                int numberOfCards,
                                const std::vector<const Card*> &cards);

    void updatePhase(GamePhase newPhase);


    // UI Logic

    void drawTable();

    void drawPlayerHand(const std::vector<const Card*> hand, qreal x, qreal y);
    void drawOtherPlayersHands(const std::vector<ClientPlayerState>& players);
    void drawOtherCardsInPlay(const std::vector<ClientPlayerState>& players);
    void drawOtherPlayersDrawAndDiscard(const std::vector<ClientPlayerState>& players);
    void drawCardsInPlay(const std::vector<const Card*> cardsInPlay, qreal x, qreal y);
    void drawStacks(quint16 cardsInDraw, quint16 cardsInDiscard, const Card* topDiscardCard);

    void clearCardsInPlay();

    void drawShop();

    void setPhaseText(GamePhase phase);
        
    void displayMessage(QString message);
    


};