/*
 * 模块名称  : UI——菜单系统
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 多级菜单框架——主菜单 + 各功能模块子菜单。
 *             负责用户交互流程控制与功能调度。
 */

#ifndef MENU_H
#define MENU_H

#include "../services/road_network.h"
#include "../services/congestion.h"
#include "../services/comparator.h"
#include "../services/file_io.h"

#include <string>

/**
 * @class MenuSystem
 * @brief 控制台多级菜单系统
 *
 * 菜单层级：
 *   主菜单
 *   ├── 1. 路网编辑
 *   │   ├── 1.1 添加城市
 *   │   ├── 1.2 删除城市
 *   │   ├── 1.3 添加道路
 *   │   ├── 1.4 删除道路
 *   │   ├── 1.5 修改道路权值
 *   │   └── 1.6 查看路网总览
 *   ├── 2. 图遍历
 *   │   ├── 2.1 DFS 深度优先遍历
 *   │   └── 2.2 BFS 广度优先遍历
 *   ├── 3. 最短路径
 *   │   ├── 3.1 Dijkstra 单源最短路径
 *   │   └── 3.2 Floyd 多源最短路径
 *   ├── 4. 最小生成树
 *   │   ├── 4.1 Prim 算法
 *   │   └── 4.2 Kruskal 算法
 *   ├── 5. 拓扑排序
 *   ├── 6. 拥堵模拟
 *   ├── 7. 结构性能对比
 *   ├── 8. 数据文件管理
 *   │   ├── 8.1 保存路网到文件
 *   │   ├── 8.2 从文件加载路网
 *   │   └── 8.3 自动加载
 *   ├── 9. 帮助 / 关于
 *   └── 0. 退出系统
 */
class MenuSystem
{
private:
    RoadNetwork *network;            /* 路网管理器 */
    CongestionSimulator *simulator;  /* 拥堵模拟器 */
    StructureComparator *comparator; /* 性能对比器 */
    bool is_running;                 /* 主循环控制 */

    bool congestion_active;          /* 是否存在当前拥堵记录 */
    int congestion_from;             /* 拥堵道路起点 */
    int congestion_to;               /* 拥堵道路终点 */
    int congestion_original_weight;  /* 拥堵前权值 */
    int congestion_congested_weight;  /* 拥堵后权值 */

    char current_file_path[256];      /* 当前加载的路网文件路径 */

public:
    MenuSystem();
    ~MenuSystem();

    /** @brief 启动菜单主循环 */
    void run();

    /** @brief 显示启动欢迎界面 */
    void show_welcome() const;

private:
    /* ========== 主菜单 ========== */
    void show_main_menu() const;
    int get_menu_choice(int min, int max) const;
    void dispatch_choice(int choice);

    /* ========== 子菜单 ========== */
    void menu_network_edit();
    void menu_traversal();
    void menu_shortest_path();
    void menu_spanning_tree();
    void menu_topological_sort();
    void menu_congestion();
    void menu_comparator();
    void menu_file_manage();
    void menu_help();

    /* ========== 初始化 ========== */
    void init_network();
    void load_default_data(); /* 首次运行加载示例路网 */
    void reset_congestion_state();
};

#endif /* MENU_H */
