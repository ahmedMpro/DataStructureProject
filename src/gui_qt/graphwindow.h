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
class QTimer;

struct AnimationStep {
    enum class Type { NodeVisit, NodeBacktrack, EdgeTraverse, EdgeCycle, NodeCycle, UnionHighlight };
    Type type{Type::NodeVisit};
    int node{-1};
    int source{-1};
    int target{-1};
};
struct AnimationStep;

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
    void advanceAnimationStep();
    void clearAnimationHighlights();
    void applyFinalCycleHighlights();
    void finalizeAnimation();
    void handleEdgeClicked(EdgeItem* edge);
    bool prepareAnimationSteps();
    bool collectDirectedSteps(int vertex, QVector<bool>& visited, QVector<bool>& recursionStack);
    bool collectUndirectedSteps();
    EdgeItem* findEdge(int source, int target) const;
    int findEdgeIndex(EdgeItem* edge) const;
    void createEdge(int source, int target);
    bool edgeAlreadyExists(int source, int target) const;
    void synchronizeGraphEdges();
    void deleteVertex(int index);
    void reindexAfterVertexRemoval(int removedIndex);
     void logCycleDetection();
    void exitDeleteMode();
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
    QPushButton* deleteEdgeButton_;
    QPushButton* deleteVertexButton_;
    NodeItem* selectedNode_{nullptr};
    int vertexCount_{0};
    bool isDirected_{false};
    QTimer* animationTimer_;
    QVector<AnimationStep> animationSteps_;
    int animationStepIndex_{0};
    bool animationRunning_{false};
    bool animationDetectedCycle_{false};
};
