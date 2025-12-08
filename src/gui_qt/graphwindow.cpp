#include "graphwindow.h"
#include "edgeitem.h"
#include "nodeitem.h"

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
#include <QVBoxLayout>
#include <QtAlgorithms>
#include <QtCore/Qt>
#include <QtGlobal>
#include <cmath>

namespace {
constexpr int kDefaultVertexCount = 5;
constexpr qreal kPi = 3.14159265358979323846;
const QString kInfoStyle = "color: #33f4ff;";
const QString kWarningStyle = "color: #ff6a8d;";
const QString kSuccessStyle = "color: #7ef279;";
const QString kResultFrameStyle = "border: 1px solid rgba(255, 255, 255, 0.15); border-radius: 14px;";
const QString kResultFontStyle = "font-family: 'Times New Roman'; font-size: 22px; font-weight: 600; text-transform: uppercase; letter-spacing: 0.05em;";
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
      resultLabel_(new QLabel())
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

    // Do not draw anything at startup
}

void GraphWindow::drawGraph()
{
    resetScene(vertexSpin_->value());
    updateStatus(tr("Nodes placed. Select two to draw edges."));
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

    const bool hasCycle = graph_.detectCycle();
    updateResultLabel(hasCycle ? tr("Cycle detected in the current graph.") : tr("Graph is acyclic."), hasCycle);
    updateStatus(hasCycle ? tr("Cycle detected.") : tr("No cycles found."));
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
