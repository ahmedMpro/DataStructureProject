#include "nodeitem.h"

#include <QBrush>
#include <QColor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QString>
#include <QtCore/Qt>

namespace {
constexpr qreal kNodeRadius = 28.0;
const QColor kNodeFill = QColor(255, 105, 180);
const QColor kNodeStroke = QColor(255, 182, 193);
const QColor kNodeTextColor = QColor(255, 255, 255);
}

NodeItem::NodeItem(int index, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsEllipseItem(-kNodeRadius, -kNodeRadius, kNodeRadius * 2, kNodeRadius * 2, parent),
      index_(index)
{
    setBrush(kNodeFill);
    setPen(QPen(kNodeStroke, 2.5));
    defaultBrush_ = brush();
    defaultPen_ = pen();
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsScenePositionChanges |
             QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);
    setZValue(1);
    textFont_.setFamily("Segoe UI");
    textFont_.setBold(true);
    textFont_.setPointSize(12);
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    dragging_ = false;
    QGraphicsEllipseItem::mousePressEvent(event);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    dragging_ = true;
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsEllipseItem::mouseReleaseEvent(event);
    if (!dragging_) {
        emit clicked(index_);
    }
    dragging_ = false;
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged) {
        emit moved(index_);
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QGraphicsEllipseItem::paint(painter, option, widget);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(kNodeTextColor);
    painter->setFont(textFont_);
    painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(index_));
}

void NodeItem::highlight(const QColor& fill, const QColor& stroke)
{
    setBrush(fill);
    setPen(QPen(stroke, 3.4));
    update();
}

void NodeItem::resetAppearance()
{
    setBrush(defaultBrush_);
    setPen(defaultPen_);
    update();
}
