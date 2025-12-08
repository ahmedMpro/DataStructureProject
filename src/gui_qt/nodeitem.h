#pragma once

#include <QObject>
#include <QFont>
#include <QGraphicsEllipseItem>
#include <QVariant>

class QGraphicsSceneMouseEvent;

class NodeItem : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT

public:
    explicit NodeItem(int index, QGraphicsItem* parent = nullptr);
    int index() const noexcept { return index_; }

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
};
