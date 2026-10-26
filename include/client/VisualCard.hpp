#pragma once
#include <QGraphicsObject>
#include <QPainter>
#include <QString>
#include <QPixmap>
#include "client/utils/Location.hpp"

class VisualCard : public QGraphicsObject {
    Q_OBJECT
public:
    VisualCard(int id, const QString& name, int cost, const QString& imagePath,Location location, QGraphicsItem *parent = nullptr);

    ~VisualCard();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    int getId() const { return m_id; }
    void setSelectedEffect(bool selected);

    Location getLocation() const;

    static int getNewId() {
        static int currentId = 0;
        return currentId++;
    }

signals:
    void clicked(int cardId);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:

    int m_id;
    QString m_name;
    int m_cost;
    bool m_isSelected{false};
    Location m_location;

    QPixmap m_image;
    
    const int CARD_WIDTH = 100;
    const int CARD_HEIGHT = 150;
};