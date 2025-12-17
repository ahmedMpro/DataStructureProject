#include "edgeitem.h"
#include "nodeitem.h"

#include <QColor>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPolygonF>
#include <QtCore/Qt>
#include <QtMath>
#include <cmath>

namespace {
const QColor kEdgeColor(54, 252, 215);
const QColor kDirectedEdgeColor(255, 105, 180);
constexpr qreal kArrowSize = 26.0;
constexpr qreal kPi = 3.14159265358979323846;
}

EdgeItem::EdgeItem(NodeItem* source, NodeItem* target, bool directed, QGraphicsItem* parent)
        : QObject(nullptr),
            QGraphicsLineItem(parent),
            source_(source),
            target_(target),
            directed_(directed),
            pen_(kEdgeColor, 3, Qt::SolidLine, Qt::RoundCap)
{
    setZValue(0);
    setDirected(directed);
        defaultPen_ = pen_;
    updatePosition();
}

void EdgeItem::updatePosition()
{
    if (!source_ || !target_) {
        return;
    }

    const QLineF connection(source_->pos(), target_->pos());
    setLine(connection);
}

void EdgeItem::setDirected(bool directed) noexcept
{
    directed_ = directed;
    pen_.setColor(directed_ ? kDirectedEdgeColor : kEdgeColor);
    defaultPen_ = pen_;
    setPen(pen_);
    update();
}

void EdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QGraphicsLineItem::paint(painter, option, widget);

    if (!directed_) {
        return;
    }

    const QLineF line = this->line();
    if (qFuzzyIsNull(line.length())) {
        return;
    }

    const qreal angle = std::atan2(-line.dy(), line.dx());
    const qreal arrowLength = kArrowSize;
    const qreal arrowWidth = kArrowSize * 0.7;
    QPointF midPoint = (line.p1() + line.p2()) / 2;
    QPointF arrowTip = midPoint + QPointF(std::cos(angle) * arrowLength / 2, -std::sin(angle) * arrowLength / 2);
    QPointF arrowBase1 = midPoint + QPointF(std::cos(angle + kPi / 2) * arrowWidth / 2, -std::sin(angle + kPi / 2) * arrowWidth / 2);
    QPointF arrowBase2 = midPoint + QPointF(std::cos(angle - kPi / 2) * arrowWidth / 2, -std::sin(angle - kPi / 2) * arrowWidth / 2);

    QPolygonF arrowHead;
    arrowHead << arrowTip << arrowBase1 << arrowBase2;
    painter->setBrush(pen_.color());
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(arrowHead);
}

void EdgeItem::highlight(const QColor& color)
{
    pen_ = defaultPen_;
    pen_.setColor(color);
    pen_.setWidthF(defaultPen_.widthF() + 1.5);
    setPen(pen_);
    update();
}

void EdgeItem::resetAppearance()
{
    pen_ = defaultPen_;
    setPen(pen_);
    update();
}

void EdgeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event && event->button() == Qt::LeftButton) {
        emit clicked(this);
    }
    QGraphicsLineItem::mousePressEvent(event);
}
