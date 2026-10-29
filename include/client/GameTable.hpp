#pragma once

#include <QWidget>
#include <QGraphicsScene>

#include "shared/abstract/Card.hpp"
#include "shared/player/Player.hpp"
#include "client/VisualCard.hpp"
#include "client/Game/ClientGameState.hpp"

#include <QtGlobal>

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

signals:
    void endTurnClicked();
    void widgetResized(const QSize &newSize);
    void phaseChanged(const QString& newPhase);
    
private slots:
    void onEndTurnClicked();

public:
    explicit GameTable(QWidget *parent = nullptr);
    ~GameTable() override;

    void updateGameState(ClientGameState newGameState);

    // Game Logic
    void startTurn();
    void changePhase(QString newPhase);
    void switchShop();
    void endTurn();

    void modifySelection(QString phase, int actions, int purchases, int money);


    // UI Logic

    void drawTable();

    void drawPlayerHand(const std::vector<const Card*> hand, qreal x, qreal y);
    void drawOtherPlayersHands(const std::vector<ClientPlayerState>& players);
    void drawOtherCardsInPlay(const std::vector<ClientPlayerState>& players);
    void drawOtherPlayersDrawAndDiscard(const std::vector<ClientPlayerState>& players);
    void drawCardsInPlay(const std::vector<const Card*> cardsInPlay, qreal x, qreal y);
    void drawStacks(quint16 cardsInDraw, quint16 cardsInDiscard, const Card* topDiscardCard);

    void drawShop(qreal x, qreal y);

    void setPhaseText(const QString& phase);
        
    void displayMessage(QString message);
    


};