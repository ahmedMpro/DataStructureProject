#pragma once

#include <QObject>
#include <QBrush>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QPen>
#include <QVariant>

class QGraphicsSceneMouseEvent;
class QColor;

class NodeItem : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT

public:
    explicit NodeItem(int index, QGraphicsItem* parent = nullptr);
    int index() const noexcept { return index_; }
    void setIndex(int index) noexcept { index_ = index; update(); }
    void highlight(const QColor& fill, const QColor& stroke);
    void resetAppearance();

signals:
    void clicked(int index);
    void moved(int index);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    int index_;
    bool dragging_{false};
    QFont textFont_;
    QBrush defaultBrush_;
    QPen defaultPen_;
};
