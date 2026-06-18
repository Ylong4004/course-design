/*
 * 模块名称  : Qt——道路管理面板
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 道路增删改——表单输入 + 道路列表。
 *             通过信号 road_changed() 通知主窗口刷新。
 */

#ifndef ROAD_PANEL_H
#define ROAD_PANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

#include "../services/road_network.h"

/**
 * @class RoadPanel
 * @brief 道路管理面板
 *
 * 功能：
 *   - 新增道路：输入起点/终点/权值
 *   - 删除道路：选中列表项删除
 *   - 修改权值：选中后输入新权值
 */
class RoadPanel : public QWidget
{
    Q_OBJECT

public:
    explicit RoadPanel(RoadNetwork *network,
                       QWidget *parent = nullptr);

    /** @brief 聚焦到新增道路输入框 */
    void focus_add_road();

signals:
    /** @brief 道路数据变更信号 */
    void road_changed();

private slots:
    void on_add_clicked();
    void on_delete_clicked();
    void on_update_clicked();
    void on_selection_changed();
    void refresh_list();

private:
    RoadNetwork   *m_network;

    /* 输入区 */
    QLineEdit *m_edit_from;
    QLineEdit *m_edit_to;
    QLineEdit *m_edit_weight;
    QPushButton *m_btn_add;
    QPushButton *m_btn_delete;
    QPushButton *m_btn_update;

    /* 列表区 */
    QListWidget *m_list;
    QLabel      *m_label_count;
};

#endif /* ROAD_PANEL_H */
