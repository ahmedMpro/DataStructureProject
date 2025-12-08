#include "edgeitem.h"
#include "nodeitem.h"

#include <QPainter>
#include <QPolygonF>
#include <QtMath>
#include <cmath>

namespace {
const QColor kEdgeColor(54, 252, 215);
const QColor kDirectedEdgeColor(255, 105, 180);
constexpr qreal kArrowSize = 26.0;
constexpr qreal kPi = 3.14159265358979323846;
}

EdgeItem::EdgeItem(NodeItem* source, NodeItem* target, bool directed, QGraphicsItem* parent)
    : QGraphicsLineItem(parent),
      source_(source),
      target_(target),
      directed_(directed),
      pen_(kEdgeColor, 3, Qt::SolidLine, Qt::RoundCap)
{
    setZValue(0);
    setDirected(directed);
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
