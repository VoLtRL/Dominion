#include "client/VisualCard.hpp"
#include <QPainter>
#include <iostream>
#include <QGraphicsSceneMouseEvent>

VisualCard::VisualCard(int id, const Card* associatedCard, const QString& name, int cost, const QString& imagePath,Location location, QGraphicsItem *parent)
    : QGraphicsObject(parent), m_id(id), m_name(name), m_cost(cost), m_location(location), m_associatedCard(associatedCard) {
    m_image = QPixmap(imagePath);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

VisualCard::~VisualCard() {
}

void VisualCard::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    event->accept();
    emit clicked(m_id);
}

QRectF VisualCard::boundingRect() const {
    return QRectF(-2, -2, CARD_WIDTH + 4, CARD_HEIGHT + 4);
}

void VisualCard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_image.isNull()) {
        painter->drawPixmap(0, 0, CARD_WIDTH, CARD_HEIGHT, m_image);
    } else{
        painter->setBrush(Qt::white);
        painter->setPen(Qt::black);
        painter->drawRect(0, 0, CARD_WIDTH, CARD_HEIGHT);
        painter->setPen(Qt::black);
        painter->drawText(QRectF(0, 0, CARD_WIDTH, 20), Qt::AlignCenter, m_name);
        painter->drawText(QRectF(0, CARD_HEIGHT - 20, CARD_WIDTH, 20), Qt::AlignCenter, QString("Cost: %1").arg(m_cost));
    }

    if (isSelected()) {
        QPen selectionPen(Qt::yellow);
        selectionPen.setWidth(4);
        painter->setPen(selectionPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(QRectF(2, 2, CARD_WIDTH - 4, CARD_HEIGHT - 4));
    }
}

Location VisualCard::getLocation() const {
    return m_location;
}