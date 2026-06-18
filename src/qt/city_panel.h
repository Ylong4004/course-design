/*
 * 模块名称  : Qt——城市管理面板
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 城市的增删查——表单输入 + 城市列表。
 *             通过信号 city_changed() 通知主窗口刷新。
 */

#ifndef CITY_PANEL_H
#define CITY_PANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

#include "../services/road_network.h"

/**
 * @class CityPanel
 * @brief 城市管理面板
 *
 * 功能：
 *   - 新增城市：输入 ID + 名称，点击添加
 *   - 删除城市：选中列表项，点击删除（级联删除道路）
 *   - 显示当前城市列表
 */
class CityPanel : public QWidget
{
    Q_OBJECT

public:
    explicit CityPanel(RoadNetwork *network,
                       QWidget *parent = nullptr);

    /** @brief 聚焦到新增城市输入框 */
    void focus_add_city();

signals:
    /** @brief 城市数据变更（增/删）信号 */
    void city_changed();

private slots:
    void on_add_clicked();
    void on_delete_clicked();
    void on_selection_changed();
    void refresh_list();

private:
    RoadNetwork   *m_network;

    /* 输入区 */
    QLineEdit *m_edit_id;
    QLineEdit *m_edit_name;
    QPushButton *m_btn_add;
    QPushButton *m_btn_delete;

    /* 列表区 */
    QListWidget *m_list;
    QLabel      *m_label_count;
};

#endif /* CITY_PANEL_H */
