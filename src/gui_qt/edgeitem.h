#pragma once

#include <QObject>
#include <QGraphicsLineItem>
#include <QPen>

class NodeItem;
class QColor;
class QGraphicsSceneMouseEvent;

class EdgeItem : public QObject, public QGraphicsLineItem {
    Q_OBJECT

public:
    explicit EdgeItem(NodeItem* source, NodeItem* target, bool directed = false, QGraphicsItem* parent = nullptr);
    void updatePosition();
    void setDirected(bool directed) noexcept;
    void highlight(const QColor& color);
    void resetAppearance();

signals:
    void clicked(EdgeItem*);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    NodeItem* source_;
    NodeItem* target_;
    bool directed_;
    QPen pen_;
    QPen defaultPen_;
};
