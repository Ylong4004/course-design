/*
 * 模块名称  : Qt——路网可视化画布实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : QGraphicsView 绘制城市节点（圆形）+ 道路（线段 + 权值标注）。
 *             自动环形布局，支持滚轮缩放。
 */

#include "network_view.h"
#include "../common/types.h"

#include <QWheelEvent>
#include <QtMath>
#include <QPen>
#include <QBrush>
#include <QFont>

/* 布局常量 */
static const double NODE_RADIUS   = 22.0;
static const double SCENE_MARGIN  = 60.0;
static const double LAYOUT_RADIUS = 200.0;

NetworkView::NetworkView(RoadNetwork *network, QWidget *parent)
    : QGraphicsView(parent),
      m_network(network),
      m_scene(new QGraphicsScene(this))
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    /* 初始场景矩形 */
    m_scene->setSceneRect(-400, -400, 800, 800);
}

NetworkView::~NetworkView()
{
}

/* ============================================================ */
/*  刷新绘制                                                    */
/* ============================================================ */

void NetworkView::refresh()
{
    m_scene->clear();

    if (!m_network) return;

    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    if (!g || g->get_vertex_count() == 0) {
        /* 空图提示 */
        QGraphicsTextItem *hint = m_scene->addText(
            "路网为空\n点击工具栏 \"新增城市\" 开始构建");
        hint->setDefaultTextColor(Qt::gray);
        hint->setPos(-80, -20);
        return;
    }

    int *ids = nullptr;
    int count = 0;
    g->get_all_vertex_ids(&ids, &count);
    if (!ids || count == 0) return;

    /* 第一步：绘制所有城市节点 */
    for (int i = 0; i < count; ++i) {
        QPointF pos = compute_position(i, count);

        City_t city;
        if (g->get_vertex(ids[i], &city) != SUCCESS) continue;

        /* 圆形节点 */
        QGraphicsEllipseItem *node = m_scene->addEllipse(
            pos.x() - NODE_RADIUS,
            pos.y() - NODE_RADIUS,
            NODE_RADIUS * 2,
            NODE_RADIUS * 2,
            QPen(Qt::darkBlue, 2.0),
            QBrush(QColor(173, 216, 230))); /* 浅蓝填充 */

        node->setZValue(2);

        /* 城市名称（ID + 名称） */
        QString label = QString::number(city.id) + "\n" + city.name;
        QGraphicsTextItem *text = m_scene->addText(label);
        text->setDefaultTextColor(Qt::black);
        QFont font;
        font.setPointSize(8);
        text->setFont(font);
        /* 居中 */
        QRectF text_rect = text->boundingRect();
        text->setPos(pos.x() - text_rect.width() / 2,
                     pos.y() + NODE_RADIUS + 4);
        text->setZValue(2);
    }

    /* 第二步：绘制所有道路 */
    for (int i = 0; i < count; ++i) {
        for (int j = 0; j < count; ++j) {
            if (i == j) continue;
            if (!g->has_edge(ids[i], ids[j])) continue;

            int weight = 0;
            g->get_edge_weight(ids[i], ids[j], &weight);

            QPointF p1 = compute_position(i, count);
            QPointF p2 = compute_position(j, count);

            /* 线段（从圆心到圆心） */
            QGraphicsLineItem *line = m_scene->addLine(
                p1.x(), p1.y(), p2.x(), p2.y(),
                QPen(Qt::darkGray, 1.5));
            line->setZValue(1);

            /* 权值标注（中点偏移） */
            QPointF mid = (p1 + p2) / 2.0;
            /* 小幅偏移避免和线段重叠 */
            QPointF offset(-10, -10);
            QGraphicsTextItem *wtext = m_scene->addText(
                QString::number(weight));
            wtext->setDefaultTextColor(Qt::red);
            QFont wfont;
            wfont.setPointSize(7);
            wfont.setBold(true);
            wtext->setFont(wfont);
            wtext->setPos(mid + offset);
            wtext->setZValue(3);
        }
    }

    delete[] ids;
}

/* ============================================================ */
/*  环形布局                                                    */
/* ============================================================ */

QPointF NetworkView::compute_position(int index, int total) const
{
    if (total <= 0) return QPointF(0, 0);

    double angle = (2.0 * M_PI * index) / total - M_PI / 2.0;
    double x = LAYOUT_RADIUS * qCos(angle);
    double y = LAYOUT_RADIUS * qSin(angle);
    return QPointF(x, y);
}

/* ============================================================ */
/*  滚轮缩放                                                    */
/* ============================================================ */

void NetworkView::wheelEvent(QWheelEvent *event)
{
    double factor = 1.15;
    if (event->angleDelta().y() < 0) {
        factor = 1.0 / factor;
    }
    scale(factor, factor);
}
