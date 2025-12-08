#pragma once

#include "Logic/graph.h"

#include <QWidget>
#include <QVector>

class EdgeItem;
class NodeItem;
class QButtonGroup;
class QGraphicsScene;
class QGraphicsView;
class QPushButton;
class QRadioButton;
class QLabel;
class QSpinBox;

struct EdgeRecord {
    EdgeItem* item;
    int source;
    int target;
};

class GraphWindow : public QWidget {
    Q_OBJECT

public:
    explicit GraphWindow(QWidget* parent = nullptr);

private slots:
    void drawGraph();
    void checkForCycle();
    void onDirectionChanged();
    void nodeClicked(int index);
    void nodeMoved(int index);

private:
    void resetScene(int vertexCount);
    void layoutNodes();
    void clearSceneContent();
    void createEdge(int source, int target);
    bool edgeAlreadyExists(int source, int target) const;
    void synchronizeGraphEdges();
    void updateResultLabel(const QString& text, bool cyclic);
    void updateStatus(const QString& text, const QString& kind = "info");
    void applyResultStyle(const QString& colorStyle);

    QGraphicsScene* scene_; 
    QGraphicsView* view_;
    QSpinBox* vertexSpin_; 
    QPushButton* drawButton_;
    QPushButton* checkButton_;
    QRadioButton* directedRadio_; 
    QRadioButton* undirectedRadio_;
    QLabel* statusLabel_;
    QLabel* resultLabel_;
    Graph graph_; 
    QVector<NodeItem*> nodes_; 
    QVector<EdgeRecord> edges_; 
    NodeItem* selectedNode_{nullptr};
    int vertexCount_{0};
    bool isDirected_{false};
};
