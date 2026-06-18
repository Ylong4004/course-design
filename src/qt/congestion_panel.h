/*
 * 模块名称  : Qt——拥堵模拟面板
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 拥堵模拟——设置/恢复拥堵道路，查看拥堵前后路径对比。
 */

#ifndef CONGESTION_PANEL_H
#define CONGESTION_PANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>

#include "../services/road_network.h"
#include "../services/congestion.h"

/**
 * @class CongestionPanel
 * @brief 拥堵模拟面板
 */
class CongestionPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CongestionPanel(RoadNetwork *network,
                             CongestionSimulator *simulator,
                             QWidget *parent = nullptr);

private slots:
    void on_set_congestion();
    void on_restore();
    void on_compare();

private:
    RoadNetwork         *m_network;
    CongestionSimulator *m_simulator;

    QLineEdit   *m_edit_from;
    QLineEdit   *m_edit_to;
    QLineEdit   *m_edit_weight;
    QLineEdit   *m_edit_start;
    QPushButton *m_btn_set;
    QPushButton *m_btn_restore;
    QPushButton *m_btn_compare;
    QTextEdit   *m_text_output;
};

#endif /* CONGESTION_PANEL_H */
