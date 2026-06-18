/*
 * 模块名称  : Qt——路网可视化画布
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 使用 QGraphicsView / QGraphicsScene 绘制城市节点与道路连线。
 *             从 RoadNetwork 读取数据，不依赖控制台输出。
 */

#ifndef NETWORK_VIEW_H
#define NETWORK_VIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>

#include "../services/road_network.h"

/**
 * @class NetworkView
 * @brief 路网可视化画布
 *
 * 绘制逻辑：
 *   - 城市节点：圆形（QGraphicsEllipseItem），半径 20px
 *   - 城市名称：文本标注（QGraphicsTextItem）
 *   - 道路：线段 + 权值标注（QGraphicsLineItem + QGraphicsTextItem）
 *   - 节点位置自动环形布局，避免重叠
 */
class NetworkView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit NetworkView(RoadNetwork *network,
                         QWidget *parent = nullptr);
    ~NetworkView();

    /** @brief 根据当前路网数据重新绘制全部节点与边 */
    void refresh();

protected:
    /** @brief 滚轮缩放 */
    void wheelEvent(QWheelEvent *event) override;

private:
    /** @brief 自动计算节点环形布局坐标 */
    QPointF compute_position(int index, int total) const;

    RoadNetwork   *m_network;
    QGraphicsScene *m_scene;
};

#endif /* NETWORK_VIEW_H */
