#include "graphwindow.h"
#include "edgeitem.h"
#include "nodeitem.h"

#include "Logic/disjoint_set.h"

#include <QBrush>
#include <QColor>
#include <QButtonGroup>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QVBoxLayout>
#include <QtAlgorithms>
#include <QtCore/Qt>
#include <QtGlobal>
#include <QCoreApplication>
#include <QDateTime>
#include <QFile>
#include <QMap>
#include <QTextStream>
#include <QTimer>
#include <cmath>

namespace {
constexpr qreal kPi = 3.14159265358979323846;
const QString kInfoStyle = "color: #33f4ff;";
const QString kWarningStyle = "color: #ff6a8d;";
const QString kSuccessStyle = "color: #7ef279;";
const QString kResultFrameStyle = "border: 1px solid rgba(255, 255, 255, 0.15); border-radius: 14px;";
const QString kResultFontStyle = "font-family: 'Times New Roman'; font-size: 22px; font-weight: 600; text-transform: uppercase; letter-spacing: 0.05em;";
const QColor kNodeVisitFill(255, 196, 238);
const QColor kNodeVisitStroke(255, 105, 180);
const QColor kNodeBacktrackFill(48, 66, 113);
const QColor kNodeBacktrackStroke(255, 182, 193);
const QColor kNodeCycleFill(255, 105, 180);
const QColor kNodeCycleStroke(255, 51, 102);
const QColor kEdgeTraverseColor(255, 214, 247);
const QColor kEdgeCycleColor(255, 82, 175);
const QColor kEdgeUnionColor(118, 241, 137);

}

GraphWindow::GraphWindow(QWidget* parent)
    : QWidget(parent),
      scene_(new QGraphicsScene(this)),
      view_(new QGraphicsView(scene_)),
      vertexSpin_(new QSpinBox()),
      drawButton_(new QPushButton(tr("Draw Graph"))),
      checkButton_(new QPushButton(tr("Check Cyclic"))),
      directedRadio_(new QRadioButton(tr("Directed"))),
      undirectedRadio_(new QRadioButton(tr("Undirected"))),
      statusLabel_(new QLabel()),
        resultLabel_(new QLabel()),
        deleteEdgeButton_(new QPushButton(tr("Delete Edge"))),
        deleteVertexButton_(new QPushButton(tr("Delete Vertex"))),
        animationTimer_(new QTimer(this))
{
    setWindowTitle(tr("Neon Cycle Explorer"));
    resize(1120, 720);
    setStyleSheet("background-color: #030712; color: #e8f4ff; QLabel { font-size: 14px; } QGroupBox { border: none; }");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(18, 18, 18, 18);

    auto* header = new QLabel(tr("Drag the glowing nodes, connect them, and test for cycles."));
    header->setStyleSheet("font-size: 24px; font-weight: 600;");
    mainLayout->addWidget(header);

    auto* controlRow = new QHBoxLayout();
    controlRow->setSpacing(12);

    vertexSpin_->setRange(1, 9999);
    vertexSpin_->clear();
    vertexSpin_->setFixedWidth(110);
    vertexSpin_->setStyleSheet("font-size: 18px; padding: 6px 10px; background: #181a2a; color: #ff69b4; border-radius: 8px; border: 1px solid #ff69b4;");

    auto* vertexLabel = new QLabel(tr("Vertex count:"));
    vertexLabel->setStyleSheet("font-size: 18px; color: #ff69b4; font-weight: bold;");

    controlRow->addWidget(vertexLabel);
    controlRow->addWidget(vertexSpin_);
    drawButton_->setMinimumHeight(38);
    drawButton_->setMinimumWidth(140);
    drawButton_->setStyleSheet("font-size: 18px; background: #181a2a; color: #ff69b4; border-radius: 8px; border: 1px solid #ff69b4;");
    controlRow->addWidget(drawButton_);

    auto* radioGroup = new QButtonGroup(this);
    radioGroup->addButton(undirectedRadio_);
    radioGroup->addButton(directedRadio_);
    undirectedRadio_->setChecked(true);
    directedRadio_->setStyleSheet("color: #e8f4ff;");
    undirectedRadio_->setStyleSheet("color: #e8f4ff;");

    auto* radioLayout = new QHBoxLayout();
    undirectedRadio_->setMinimumHeight(32);
    undirectedRadio_->setStyleSheet("font-size: 18px; color: #e8f4ff; background: #181a2a; border-radius: 8px; padding: 4px 12px;");
    directedRadio_->setMinimumHeight(32);
    directedRadio_->setStyleSheet("font-size: 18px; color: #ff69b4; background: #181a2a; border-radius: 8px; padding: 4px 12px;");
    radioLayout->addWidget(undirectedRadio_);
    radioLayout->addWidget(directedRadio_);
    radioLayout->addStretch();

    controlRow->addLayout(radioLayout);
    checkButton_->setMinimumHeight(38);
    checkButton_->setMinimumWidth(140);
    checkButton_->setStyleSheet("font-size: 18px; background: #181a2a; color: #ff69b4; border-radius: 8px; border: 1px solid #ff69b4;");
    controlRow->addWidget(checkButton_);

    deleteEdgeButton_->setCheckable(true);
    deleteEdgeButton_->setMinimumHeight(38);
    deleteEdgeButton_->setMinimumWidth(140);
    deleteEdgeButton_->setStyleSheet("font-size: 18px; background: #2a142a; color: #ff9ec7; border-radius: 8px; border: 1px solid #ff69b4;");
    controlRow->addWidget(deleteEdgeButton_);

    deleteVertexButton_->setCheckable(true);
    deleteVertexButton_->setMinimumHeight(38);
    deleteVertexButton_->setMinimumWidth(140);
    deleteVertexButton_->setStyleSheet("font-size: 18px; background: #2a142a; color: #ff9ec7; border-radius: 8px; border: 1px solid #ff69b4;");
    controlRow->addWidget(deleteVertexButton_);
    controlRow->addStretch();

    mainLayout->addLayout(controlRow);

    view_->setRenderHint(QPainter::Antialiasing, true);
    view_->setStyleSheet("border-radius: 24px; background: #02040c; border: 1px solid #1d2b47;");
    view_->setMinimumHeight(460);
    view_->setDragMode(QGraphicsView::NoDrag);
    scene_->setSceneRect(0, 0, 1020, 460);
    scene_->setBackgroundBrush(QBrush(QColor(2, 5, 18)));

    mainLayout->addWidget(view_);

    statusLabel_->setStyleSheet(kInfoStyle);
    statusLabel_->setText(tr("Configure the graph and start drawing edges."));
    mainLayout->addWidget(statusLabel_);

    resultLabel_->setAlignment(Qt::AlignCenter);
    resultLabel_->setFixedHeight(80);
    applyResultStyle(kInfoStyle);
    resultLabel_->setText(tr("Waiting for your graph...").toUpper());
    mainLayout->addWidget(resultLabel_);

    connect(drawButton_, &QPushButton::clicked, this, &GraphWindow::drawGraph);
    connect(checkButton_, &QPushButton::clicked, this, &GraphWindow::checkForCycle);
    connect(directedRadio_, &QRadioButton::toggled, this, &GraphWindow::onDirectionChanged);
    connect(undirectedRadio_, &QRadioButton::toggled, this, &GraphWindow::onDirectionChanged);
    connect(deleteEdgeButton_, &QPushButton::toggled, this, [this](bool checked) {
        updateStatus(checked ? tr("Delete mode active. Click an edge to remove it.") : tr("Delete mode off."));
        if (checked) {
            deleteVertexButton_->setChecked(false);
        }
    });
    connect(deleteVertexButton_, &QPushButton::toggled, this, [this](bool checked) {
        updateStatus(checked ? tr("Delete vertex mode active. Click a node to remove it.") : tr("Vertex delete mode off."));
        if (checked) {
            deleteEdgeButton_->setChecked(false);
        }
    });
    animationTimer_->setInterval(450);
    connect(animationTimer_, &QTimer::timeout, this, &GraphWindow::advanceAnimationStep);

    // Do not draw anything at startup
}

void GraphWindow::drawGraph()
{
    const int count = vertexSpin_->value();
    if (count <= 0) {
        updateStatus(tr("Enter the number of nodes you want to draw."), "warning");
        return;
    }

    deleteEdgeButton_->setChecked(false);
    deleteVertexButton_->setChecked(false);
    clearAnimationHighlights();
    animationSteps_.clear();
    animationDetectedCycle_ = false;

    resetScene(count);
    updateStatus(tr("Nodes placed. Select two to draw edges."));
}

void GraphWindow::exitDeleteMode()
{
    if (deleteEdgeButton_->isChecked()) {
        deleteEdgeButton_->setChecked(false);
    }
    if (deleteVertexButton_->isChecked()) {
        deleteVertexButton_->setChecked(false);
    }
}

void GraphWindow::resetScene(int vertexCount)
{
    vertexCount_ = vertexCount;
    selectedNode_ = nullptr;
    clearSceneContent();
    nodeMoved(-1);

    scene_->setSceneRect(0, 0, 1020, 460);
    graph_.configure(vertexCount, isDirected_);

    for (int index = 0; index < vertexCount; ++index) {
        auto* node = new NodeItem(index);
        scene_->addItem(node);
        nodes_.append(node);
        connect(node, &NodeItem::clicked, this, &GraphWindow::nodeClicked);
        connect(node, &NodeItem::moved, this, &GraphWindow::nodeMoved);
    }

    layoutNodes();
    updateStatus(tr("Tap two nodes to draw an edge."));
    resultLabel_->setText(tr("Awaiting connections...").toUpper());
    applyResultStyle(kInfoStyle);
}

void GraphWindow::clearSceneContent()
{
    for (EdgeRecord& record : edges_) {
        delete record.item;
    }
    edges_.clear();
    qDeleteAll(nodes_);
    nodes_.clear();
    graph_.configure(0, false);
}

void GraphWindow::layoutNodes()
{
    if (nodes_.isEmpty()) {
        return;
    }

    const QRectF bounds = scene_->sceneRect();
    const QPointF center = bounds.center();
    const qreal radius = qMin(bounds.width(), bounds.height()) / 2 - 90;

    for (int index = 0; index < nodes_.size(); ++index) {
        const qreal theta = (static_cast<qreal>(index) / nodes_.size()) * 2 * kPi;
        const qreal x = center.x() + std::cos(theta) * radius;
        const qreal y = center.y() + std::sin(theta) * radius;
        nodes_.at(index)->setPos(x, y);
    }

    for (EdgeRecord& record : edges_) {
        record.item->updatePosition();
    }
}

void GraphWindow::checkForCycle()
{
    if (vertexCount_ == 0) {
        updateStatus(tr("Please draw nodes before checking."), "warning");
        return;
    }
    if (animationRunning_) {
        updateStatus(tr("Animation already running. Wait for it to finish."), "warning");
        return;
    }

    exitDeleteMode();

    animationSteps_.clear();
    animationStepIndex_ = 0;
    prepareAnimationSteps();
    animationDetectedCycle_ = graph_.detectCycle();
    logCycleDetection();
    animationRunning_ = true;
    drawButton_->setEnabled(false);
    checkButton_->setEnabled(false);
    deleteEdgeButton_->setEnabled(false);
    deleteVertexButton_->setEnabled(false);
    resultLabel_->setText(tr("Analyzing...").toUpper());
    applyResultStyle(kInfoStyle);
    animationTimer_->start();
    updateStatus(tr("Animating cycle detection..."));
}

void GraphWindow::advanceAnimationStep()
{
    if (animationStepIndex_ >= animationSteps_.size()) {
        finalizeAnimation();
        return;
    }

    const AnimationStep step = animationSteps_.at(animationStepIndex_++);
    clearAnimationHighlights();

    switch (step.type) {
    case AnimationStep::Type::NodeVisit:
        if (step.node >= 0 && step.node < nodes_.size()) {
            nodes_.at(step.node)->highlight(kNodeVisitFill, kNodeVisitStroke);
            updateStatus(tr("Visiting node %1.").arg(step.node + 1));
        }
        break;
    case AnimationStep::Type::NodeBacktrack:
        if (step.node >= 0 && step.node < nodes_.size()) {
            nodes_.at(step.node)->highlight(kNodeBacktrackFill, kNodeBacktrackStroke);
            updateStatus(tr("Backtracking from node %1.").arg(step.node + 1));
        }
        break;
    case AnimationStep::Type::EdgeTraverse:
        if (step.source >= 0 && step.source < nodes_.size()) {
            nodes_.at(step.source)->highlight(kNodeVisitFill, kNodeVisitStroke);
        }
        if (step.target >= 0 && step.target < nodes_.size()) {
            nodes_.at(step.target)->highlight(kNodeVisitFill, kNodeVisitStroke);
        }
        if (EdgeItem* edge = findEdge(step.source, step.target)) {
            edge->highlight(kEdgeTraverseColor);
        }
        updateStatus(tr("Exploring edge %1 → %2.").arg(step.source + 1).arg(step.target + 1));
        break;
    case AnimationStep::Type::EdgeCycle:
        if (EdgeItem* edge = findEdge(step.source, step.target)) {
            edge->highlight(kEdgeCycleColor);
        }
        updateStatus(tr("Cycle edge spotted between %1 and %2.").arg(step.source + 1).arg(step.target + 1));
        break;
    case AnimationStep::Type::NodeCycle:
        if (step.node >= 0 && step.node < nodes_.size()) {
            nodes_.at(step.node)->highlight(kNodeCycleFill, kNodeCycleStroke);
            updateStatus(tr("Node %1 is part of the cycle.").arg(step.node + 1));
        }
        break;
    case AnimationStep::Type::UnionHighlight:
        if (step.source >= 0 && step.source < nodes_.size()) {
            nodes_.at(step.source)->highlight(kNodeVisitFill, kNodeVisitStroke);
        }
        if (step.target >= 0 && step.target < nodes_.size()) {
            nodes_.at(step.target)->highlight(kNodeVisitFill, kNodeVisitStroke);
        }
        if (EdgeItem* edge = findEdge(step.source, step.target)) {
            edge->highlight(kEdgeUnionColor);
        }
        updateStatus(tr("Merging sets for edge %1 → %2.").arg(step.source + 1).arg(step.target + 1));
        break;
    }
}

void GraphWindow::finalizeAnimation()
{
    animationTimer_->stop();
    animationRunning_ = false;
    drawButton_->setEnabled(true);
    checkButton_->setEnabled(true);
    deleteEdgeButton_->setEnabled(true);
    deleteVertexButton_->setEnabled(true);
    clearAnimationHighlights();
    applyFinalCycleHighlights();
    updateResultLabel(animationDetectedCycle_ ? tr("Cycle detected in the current graph.")
                                              : tr("Graph is acyclic."),
                      animationDetectedCycle_);
    updateStatus(animationDetectedCycle_ ? tr("Cycle detected.") : tr("No cycles found."));
}

void GraphWindow::clearAnimationHighlights()
{
    for (NodeItem* node : nodes_) {
        node->resetAppearance();
    }
    for (EdgeRecord& record : edges_) {
        record.item->resetAppearance();
    }
}

void GraphWindow::applyFinalCycleHighlights()
{
    for (const AnimationStep& step : animationSteps_) {
        if (step.type == AnimationStep::Type::EdgeCycle) {
            if (EdgeItem* edge = findEdge(step.source, step.target)) {
                edge->highlight(kEdgeCycleColor);
            }
        }
        if (step.type == AnimationStep::Type::NodeCycle && step.node >= 0 && step.node < nodes_.size()) {
            nodes_.at(step.node)->highlight(kNodeCycleFill, kNodeCycleStroke);
        }
    }
}

bool GraphWindow::prepareAnimationSteps()
{
    if (vertexCount_ <= 0) {
        return false;
    }

    animationSteps_.clear();

    if (isDirected_) {
        QVector<bool> visited(vertexCount_, false);
        QVector<bool> recursionStack(vertexCount_, false);
        for (int vertex = 0; vertex < vertexCount_; ++vertex) {
            if (!visited[vertex]) {
                if (collectDirectedSteps(vertex, visited, recursionStack)) {
                    break;
                }
            }
        }
    } else {
        collectUndirectedSteps();
    }

    return !animationSteps_.isEmpty();
}

bool GraphWindow::collectDirectedSteps(int vertex, QVector<bool>& visited, QVector<bool>& recursionStack)
{
    auto adjacency = graph_.getAdjacencyList();
    animationSteps_.append({AnimationStep::Type::NodeVisit, vertex});
    visited[vertex] = true;
    recursionStack[vertex] = true;

    for (int neighbor : adjacency[vertex]) {
        animationSteps_.append({AnimationStep::Type::EdgeTraverse, -1, vertex, neighbor});
        if (!visited[neighbor]) {
            if (collectDirectedSteps(neighbor, visited, recursionStack)) {
                animationSteps_.append({AnimationStep::Type::EdgeCycle, -1, vertex, neighbor});
                animationSteps_.append({AnimationStep::Type::NodeCycle, neighbor});
                return true;
            }
        } else if (recursionStack[neighbor]) {
            animationSteps_.append({AnimationStep::Type::EdgeCycle, -1, vertex, neighbor});
            animationSteps_.append({AnimationStep::Type::NodeCycle, neighbor});
            return true;
        }
    }

    recursionStack[vertex] = false;
    animationSteps_.append({AnimationStep::Type::NodeBacktrack, vertex});
    return false;
}

bool GraphWindow::collectUndirectedSteps()
{
    auto adjacency = graph_.getAdjacencyList();
    DisjointSet set(vertexCount_);

    for (int source = 0; source < vertexCount_; ++source) {
        for (int target : adjacency[source]) {
            if (source >= target) {
                continue;
            }

            animationSteps_.append({AnimationStep::Type::EdgeTraverse, -1, source, target});
            const int rootSource = set.find(source);
            const int rootTarget = set.find(target);

            if (rootSource == rootTarget) {
                animationSteps_.append({AnimationStep::Type::EdgeCycle, -1, source, target});
                animationSteps_.append({AnimationStep::Type::NodeCycle, target});
                return true;
            }

            set.unionSets(rootSource, rootTarget);
            animationSteps_.append({AnimationStep::Type::UnionHighlight, -1, source, target});
        }
    }

    return false;
}

EdgeItem* GraphWindow::findEdge(int source, int target) const
{
    for (const EdgeRecord& record : edges_) {
        if (record.source == source && record.target == target) {
            return record.item;
        }
        if (!isDirected_ && record.source == target && record.target == source) {
            return record.item;
        }
    }
    return nullptr;
}

int GraphWindow::findEdgeIndex(EdgeItem* edge) const
{
    for (int index = 0; index < edges_.size(); ++index) {
        if (edges_.at(index).item == edge) {
            return index;
        }
    }
    return -1;
}

void GraphWindow::handleEdgeClicked(EdgeItem* edge)
{
    if (!edge || !deleteEdgeButton_->isChecked() || animationRunning_) {
        return;
    }

    const int index = findEdgeIndex(edge);
    if (index < 0) {
        return;
    }

    const EdgeRecord record = edges_.takeAt(index);
    graph_.removeEdge(record.source, record.target);
    scene_->removeItem(record.item);
    record.item->deleteLater();
    updateStatus(tr("Edge removed. Run a cycle check to see the updated graph."));
    resultLabel_->setText(tr("Edge removed.").toUpper());
    applyResultStyle(kInfoStyle);
}

void GraphWindow::deleteVertex(int index)
{
    if (animationRunning_) {
        return;
    }
    if (index < 0 || index >= nodes_.size()) {
        return;
    }

    // Remove incident edges first.
    for (int i = edges_.size() - 1; i >= 0; --i) {
        const EdgeRecord& record = edges_.at(i);
        if (record.source == index || record.target == index) {
            EdgeRecord removed = edges_.takeAt(i);
            graph_.removeEdge(removed.source, removed.target);
            scene_->removeItem(removed.item);
            removed.item->deleteLater();
        }
    }

    NodeItem* node = nodes_.takeAt(index);
    scene_->removeItem(node);
    node->deleteLater();

    vertexCount_ = nodes_.size();
    selectedNode_ = nullptr;

    reindexAfterVertexRemoval(index);
    synchronizeGraphEdges();
    layoutNodes();
    updateStatus(tr("Vertex %1 deleted. Remaining vertices reindexed.").arg(index + 1));
    resultLabel_->setText(tr("Vertex removed.").toUpper());
    applyResultStyle(kInfoStyle);
}

void GraphWindow::reindexAfterVertexRemoval(int removedIndex)
{
    for (int i = removedIndex; i < nodes_.size(); ++i) {
        nodes_.at(i)->setIndex(i);
    }

    for (EdgeRecord& record : edges_) {
        if (record.source > removedIndex) {
            --record.source;
        }
        if (record.target > removedIndex) {
            --record.target;
        }
        record.item->updatePosition();
    }
}

void GraphWindow::onDirectionChanged()
{
    isDirected_ = directedRadio_->isChecked();
    for (EdgeRecord& record : edges_) {
        record.item->setDirected(isDirected_);
        record.item->updatePosition();
    }

    synchronizeGraphEdges();
    updateStatus(isDirected_ ? tr("Running directed checks.") : tr("Running undirected checks."));
}

void GraphWindow::nodeClicked(int index)
{
    if (animationRunning_) {
        updateStatus(tr("Wait for the animation to finish."), "warning");
        return;
    }

    if (deleteVertexButton_->isChecked()) {
        deleteVertex(index);
        return;
    }

    if (index < 0 || index >= nodes_.size()) {
        return;
    }

    NodeItem* node = nodes_.at(index);
    if (selectedNode_ == node) {
        createEdge(index, index);
        selectedNode_->setSelected(false);
        selectedNode_ = nullptr;
        return;
    }

    if (!selectedNode_) {
        selectedNode_ = node;
        selectedNode_->setSelected(true);
        updateStatus(tr("Selected node %1.").arg(index + 1));
        return;
    }

    createEdge(selectedNode_->index(), index);
    selectedNode_->setSelected(false);
    selectedNode_ = nullptr;
}

void GraphWindow::nodeMoved(int)
{
    for (EdgeRecord& record : edges_) {
        record.item->updatePosition();
    }
}

void GraphWindow::createEdge(int source, int target)
{
    if (!edgeAlreadyExists(source, target)) {
        auto* edge = new EdgeItem(nodes_.at(source), nodes_.at(target), isDirected_);
        scene_->addItem(edge);
        edge->setAcceptedMouseButtons(Qt::LeftButton);
        connect(edge, &EdgeItem::clicked, this, &GraphWindow::handleEdgeClicked);
        edges_.append({edge, source, target});
        graph_.addEdge(source, target);
        updateStatus(tr("Edge drawn. Drag nodes to reshape the drawing."));
        resultLabel_->setText(tr("Tap Check Cyclic when ready.").toUpper());
        applyResultStyle(kInfoStyle);
    } else {
        updateStatus(tr("Edge already exists."), "warning");
    }
}

bool GraphWindow::edgeAlreadyExists(int source, int target) const
{
    for (const EdgeRecord& record : edges_) {
        if (isDirected_) {
            if (record.source == source && record.target == target) {
                return true;
            }
        } else if ((record.source == source && record.target == target) ||
                   (record.source == target && record.target == source)) {
            return true;
        }
    }

    return false;
}

void GraphWindow::synchronizeGraphEdges()
{
    graph_.configure(vertexCount_, isDirected_);
    // Only add edges that match the current direction mode and visual direction
    for (const EdgeRecord& record : edges_) {
        if (isDirected_) {
            // Only add the edge as drawn (source -> target)
            graph_.addEdge(record.source, record.target);
        } else {
            // For undirected, add both directions (if not already present)
            graph_.addEdge(record.source, record.target);
            graph_.addEdge(record.target, record.source);
        }
    }
}

void GraphWindow::updateResultLabel(const QString& text, bool cyclic)
{
    resultLabel_->setText(text.toUpper());
    if (cyclic) {
        applyResultStyle(kWarningStyle);
    } else {
        applyResultStyle(kSuccessStyle);
    }
}

void GraphWindow::updateStatus(const QString& text, const QString& kind)
{
    statusLabel_->setText(text);
    if (kind == "warning") {
        statusLabel_->setStyleSheet(kWarningStyle);
    } else {
        statusLabel_->setStyleSheet(kInfoStyle);
    }
}

void GraphWindow::applyResultStyle(const QString& colorStyle)
{
    resultLabel_->setStyleSheet(kResultFrameStyle + kResultFontStyle + colorStyle);
}

void GraphWindow::logCycleDetection()
{
    if (vertexCount_ <= 0) {
        return;
    }

    const QString logPath = QCoreApplication::applicationDirPath() + "/cycle_detection_log.txt";
    QFile file(logPath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    if (file.size() == 0) {
        stream << "**CYCLE DETECTION**\n\n";
    }

    stream << "--------------------------------------------\n";
    stream << tr("Timestamp: %1\n").arg(QDateTime::currentDateTime().toString(Qt::ISODate));
    stream << tr("Vertex count: %1\n").arg(vertexCount_);
    stream << tr("Cycle detected: %1\n").arg(animationDetectedCycle_ ? tr("Yes") : tr("No"));
    stream << "Sets:\n";

    auto adjacency = graph_.getAdjacencyList();
    DisjointSet disjointSet(vertexCount_);
    for (int source = 0; source < adjacency.size(); ++source) {
        for (int target : adjacency[source]) {
            const int rootSource = disjointSet.find(source);
            const int rootTarget = disjointSet.find(target);
            disjointSet.unionSets(rootSource, rootTarget);
        }
    }

    QMap<int, QVector<int>> clusters;
    for (int vertex = 0; vertex < vertexCount_; ++vertex) {
        const int root = disjointSet.find(vertex);
        clusters[root].append(vertex);
    }

    int setIndex = 1;
    for (auto it = clusters.cbegin(); it != clusters.cend(); ++it, ++setIndex) {
        const QVector<int>& members = it.value();
        QStringList memberNames;
        for (int vertex : members) {
            memberNames << QString::number(vertex);
        }
        stream << tr("  Set %1: %2\n").arg(setIndex).arg(memberNames.isEmpty() ? tr("None") : memberNames.join(", "));
    }

    stream << "Vertices:\n";
    for (int vertex = 0; vertex < adjacency.size(); ++vertex) {
        QStringList neighbors;
        for (int neighbor : adjacency[vertex]) {
            neighbors << QString::number(neighbor);
        }
        stream << tr("  Vertex %1: %2\n").arg(vertex).arg(neighbors.isEmpty() ? tr("None") : neighbors.join(", "));
    }

    stream << "\n";
}
