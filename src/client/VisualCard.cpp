#include "client/VisualCard.hpp"
#include <QPainter>
#include <iostream>

VisualCard::VisualCard(int id, const QString& name, int cost, const QString& imagePath,Location location, QGraphicsItem *parent)
    : QGraphicsObject(parent), m_id(id), m_name(name), m_cost(cost), m_location(location) {
    m_image = QPixmap(imagePath);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

VisualCard::~VisualCard() {
}

QRectF VisualCard::boundingRect() const {
    return QRectF(0, 0, CARD_WIDTH, CARD_HEIGHT);
}

void VisualCard::setSelectedEffect(bool selected) {
    m_isSelected = selected;
    update();
}

void VisualCard::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event);
    emit clicked(m_id);
}

void VisualCard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect = boundingRect();

    painter->setBrush(m_isSelected ? Qt::yellow : Qt::white);
    painter->drawRect(m_isSelected ? rect.adjusted(-2, -2, 2, 2) : rect);

    if (!m_image.isNull()) {
        painter->drawPixmap(0, 0, CARD_WIDTH, CARD_HEIGHT, m_image);
    } else{
        painter->setPen(Qt::black);
        painter->drawText(QRectF(0, 0, CARD_WIDTH, 20), Qt::AlignCenter, m_name);
        painter->drawText(QRectF(0, CARD_HEIGHT - 20, CARD_WIDTH, 20), Qt::AlignCenter, QString("Cost: %1").arg(m_cost));
    }


}

Location VisualCard::getLocation() const {
    return m_location;
}