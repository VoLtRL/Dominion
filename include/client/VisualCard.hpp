#pragma once
#include <QGraphicsObject>
#include <QPainter>
#include <QString>
#include <QPixmap>
#include "client/utils/Location.hpp"
#include "shared/abstract/Card.hpp"

class VisualCard : public QGraphicsObject {
    Q_OBJECT

public:
    VisualCard(int id, const Card* associatedCard, const QString& name, int cost, const QString& imagePath,Location location, QGraphicsItem *parent = nullptr);

    ~VisualCard();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int getId() const { return m_id; }
    Location getLocation() const;

    const Card* getAssociatedCard() const { return m_associatedCard; }

    static int getNewId() {
        static int currentId = 0;
        return currentId++;
    }

signals:
    void clicked(int cardId);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        Q_UNUSED(event);
    }

private:
    const Card* m_associatedCard;

    int m_id;
    QString m_name;
    int m_cost;
    Location m_location;

    QPixmap m_image;
    
    const int CARD_WIDTH = 100;
    const int CARD_HEIGHT = 150;
};