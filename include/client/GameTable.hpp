#pragma once

#include <QWidget>
#include <QGraphicsScene>

#include "shared/abstract/Card.hpp"
#include "client/VisualCard.hpp"

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

    QVector<VisualCard*> visualCardsInHand;
    QVector<VisualCard*> visualCardsInPlay;

signals:
    void endTurnClicked();

private slots:
    void onEndTurnClicked();

public:
    explicit GameTable(QWidget *parent = nullptr);
    ~GameTable() override;

    void drawPlayerHand(const std::vector<const Card*> hand, qreal x, qreal y);
    void drawCardsInPlay(const std::vector<const Card*> cardsInPlay, qreal x, qreal y);
    void drawStacks(quint16 cardsInDraw, quint16 cardsInDiscard);

    void displayMessage(QString message);
    


};