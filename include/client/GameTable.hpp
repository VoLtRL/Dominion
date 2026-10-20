#pragma once

#include <QWidget>
#include <QGraphicsScene>

#include "shared/abstract/Card.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class GameTable; }
QT_END_NAMESPACE

class GameTable : public QWidget {
    Q_OBJECT
private:
    Ui::GameTable *ui;
    QGraphicsScene *scene;

signals:
    void endTurnClicked();

private slots:
    void onEndTurnClicked();

public:
    explicit GameTable(QWidget *parent = nullptr);
    ~GameTable() override;

    void drawPlayerHand(const std::vector<const Card*> hand);
    void drawCardsInPlay(const std::vector<const Card*> cardsInPlay);


};