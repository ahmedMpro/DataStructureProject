#pragma once

#include <QGraphicsLineItem>
#include <QPen>

class NodeItem;

class EdgeItem : public QGraphicsLineItem {
public:
    EdgeItem(NodeItem* source, NodeItem* target, bool directed = false, QGraphicsItem* parent = nullptr);
    void updatePosition();
    void setDirected(bool directed) noexcept;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    NodeItem* source_;
    NodeItem* target_;
    bool directed_;
    QPen pen_;
};
