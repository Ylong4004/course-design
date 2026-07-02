/*
 * 模块名称  : Qt——应用入口
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : Qt GUI 模式的主窗口与应用入口。
 *             直接复用 services/ 层，不依赖 ui/ 与 cli/ 模块。
 */

#ifndef QT_APP_H
#define QT_APP_H

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QAction>
#include <QMenuBar>
#include <QLabel>

#include "../services/road_network.h"
#include "../services/congestion.h"
#include "../services/comparator.h"
#include "../common/types.h"

class NetworkView;
class CityPanel;
class RoadPanel;
class AlgorithmPanel;
class CongestionPanel;
class ComparePanel;

/**
 * @brief  Qt 模式总入口——替代 QApplication 的启动封装
 * @param  argc  命令行参数个数
 * @param  argv  命令行参数数组
 * @return 应用程序退出码
 */
int qt_run(int argc, char **argv);

/**
 * @class MainWindow
 * @brief Qt GUI 主窗口
 *
 * 布局结构：
 *   菜单栏 / 工具栏
 *   ├── 左侧：路网可视化画布（QGraphicsView）
 *   ├── 右侧：可停靠面板区
 *   │   ├── 城市管理面板
 *   │   ├── 道路管理面板
 *   │   ├── 算法面板
 *   │   ├── 拥堵模拟面板
 *   │   └── 性能对比面板
 *   状态栏：城市数 / 道路数 / 图类型
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    /* ---------- 工具栏动作 ---------- */
    void on_add_city();
    void on_add_road();
    void on_save_file();
    void on_load_file();
    void on_refresh_view();

    /* ---------- 数据变更响应 ---------- */
    void on_network_changed();

private:
    /* ---------- 初始化 ---------- */
    void create_actions();
    void create_toolbar();
    void create_statusbar();
    void create_dock_panels();
    void init_services();
    void load_default_data();
    void refresh_runtime_services();
    int load_network_file(const char *path);

    /* ---------- 成员 ---------- */
    /* 服务层（Qt 独有实例） */
    RoadNetwork          *m_network;
    CongestionSimulator  *m_simulator;
    StructureComparator  *m_comparator;

    /* 可视化 */
    NetworkView          *m_network_view;

    /* 面板 */
    CityPanel            *m_city_panel;
    RoadPanel            *m_road_panel;
    AlgorithmPanel       *m_algo_panel;
    CongestionPanel      *m_congest_panel;
    ComparePanel         *m_compare_panel;

    /* 工具栏动作 */
    QAction *m_act_add_city;
    QAction *m_act_add_road;
    QAction *m_act_save;
    QAction *m_act_load;
    QAction *m_act_refresh;

    /* 状态栏 */
    QLabel *m_status_info;
};

#endif /* QT_APP_H */
