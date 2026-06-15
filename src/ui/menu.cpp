/*
 * 模块名称  : UI——菜单系统
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 多级菜单框架——主菜单 + 各功能模块子菜单。
 *             负责用户交互流程控制与功能调度。
 */

#include "menu.h"

#include "formatter.h"
#include "validator.h"

#include <iostream>

MenuSystem::MenuSystem()
    : network(nullptr), simulator(nullptr), comparator(nullptr),
      is_running(false) {
}

MenuSystem::~MenuSystem() {
    network = nullptr;
    simulator = nullptr;
    comparator = nullptr;
}

void MenuSystem::run() {
    is_running = true;
    init_network();
    load_default_data();

    while (is_running) {
        show_main_menu();
        const int choice = get_menu_choice(0, 9);
        dispatch_choice(choice);
    }
}

void MenuSystem::show_welcome() const {
    Formatter::print_title("城市交通路网分析系统");
    Formatter::print_info("这是 UI 层菜单骨架版本，已完成格式化、校验和多级菜单框架。");
    Formatter::print_info("后续可在服务层实现后继续接入路网编辑、算法分析和数据持久化。");
}

void MenuSystem::show_main_menu() const {
    Formatter::print_sub_title("主菜单");
    std::cout << " 1. 路网编辑" << std::endl;
    std::cout << " 2. 图遍历" << std::endl;
    std::cout << " 3. 最短路径" << std::endl;
    std::cout << " 4. 最小生成树" << std::endl;
    std::cout << " 5. 拓扑排序" << std::endl;
    std::cout << " 6. 拥堵模拟" << std::endl;
    std::cout << " 7. 结构性能对比" << std::endl;
    std::cout << " 8. 数据文件管理" << std::endl;
    std::cout << " 9. 帮助 / 关于" << std::endl;
    std::cout << " 0. 退出系统" << std::endl;
}

int MenuSystem::get_menu_choice(int min, int max) const {
    return Validator::read_int_safe("请选择功能编号: ", min, max);
}

void MenuSystem::dispatch_choice(int choice) {
    switch (choice) {
    case 0:
        is_running = false;
        Formatter::print_success("系统即将退出。");
        break;
    case 1:
        menu_network_edit();
        break;
    case 2:
        menu_traversal();
        break;
    case 3:
        menu_shortest_path();
        break;
    case 4:
        menu_spanning_tree();
        break;
    case 5:
        menu_topological_sort();
        break;
    case 6:
        menu_congestion();
        break;
    case 7:
        menu_comparator();
        break;
    case 8:
        menu_file_manage();
        break;
    case 9:
        menu_help();
        break;
    default:
        Formatter::print_warning("无效菜单项，请重新选择。");
        break;
    }
}

void MenuSystem::menu_network_edit() {
    Formatter::print_sub_title("路网编辑");
    std::cout << " 1. 新增城市" << std::endl;
    std::cout << " 2. 删除城市" << std::endl;
    std::cout << " 3. 新增道路" << std::endl;
    std::cout << " 4. 删除道路" << std::endl;
    std::cout << " 5. 修改道路权值" << std::endl;
    std::cout << " 6. 查看路网总览" << std::endl;
    std::cout << " 0. 返回主菜单" << std::endl;

    const int choice = get_menu_choice(0, 6);
    if (choice == 0) {
        return;
    }

    Formatter::print_warning("路网编辑的业务实现尚未接入服务层，当前仅保留菜单框架。");
    Formatter::pause();
}

void MenuSystem::menu_traversal() {
    Formatter::print_sub_title("图遍历");
    std::cout << " 1. DFS 深度优先遍历" << std::endl;
    std::cout << " 2. BFS 广度优先遍历" << std::endl;
    std::cout << " 0. 返回主菜单" << std::endl;

    const int choice = get_menu_choice(0, 2);
    if (choice == 0) {
        return;
    }

    Formatter::print_warning("遍历算法入口已保留，当前 UI 仅提供调度框架。");
    Formatter::pause();
}

void MenuSystem::menu_shortest_path() {
    Formatter::print_sub_title("最短路径");
    std::cout << " 1. Dijkstra 单源最短路径" << std::endl;
    std::cout << " 2. Floyd 多源最短路径" << std::endl;
    std::cout << " 0. 返回主菜单" << std::endl;

    const int choice = get_menu_choice(0, 2);
    if (choice == 0) {
        return;
    }

    Formatter::print_warning("最短路径功能暂未连接到服务实现。");
    Formatter::pause();
}

void MenuSystem::menu_spanning_tree() {
    Formatter::print_sub_title("最小生成树");
    std::cout << " 1. Prim 算法" << std::endl;
    std::cout << " 2. Kruskal 算法" << std::endl;
    std::cout << " 0. 返回主菜单" << std::endl;

    const int choice = get_menu_choice(0, 2);
    if (choice == 0) {
        return;
    }

    Formatter::print_warning("最小生成树功能暂未连接到服务实现。");
    Formatter::pause();
}

void MenuSystem::menu_topological_sort() {
    Formatter::print_sub_title("拓扑排序");
    Formatter::print_warning("拓扑排序入口已保留，后续接入算法实现即可使用。");
    Formatter::pause();
}

void MenuSystem::menu_congestion() {
    Formatter::print_sub_title("拥堵模拟");
    std::cout << " 1. 修改道路权值" << std::endl;
    std::cout << " 2. 恢复全部修改" << std::endl;
    std::cout << " 3. 查看修改记录" << std::endl;
    std::cout << " 4. 拥堵前后对比分析" << std::endl;
    std::cout << " 0. 返回主菜单" << std::endl;

    const int choice = get_menu_choice(0, 4);
    if (choice == 0) {
        return;
    }

    Formatter::print_warning("拥堵模拟器尚未接入服务层实现。");
    Formatter::pause();
}

void MenuSystem::menu_comparator() {
    Formatter::print_sub_title("结构性能对比");
    Formatter::print_warning("性能对比器当前仅保留 UI 入口。");
    Formatter::pause();
}

void MenuSystem::menu_file_manage() {
    Formatter::print_sub_title("数据文件管理");
    std::cout << " 1. 保存路网到文件" << std::endl;
    std::cout << " 2. 从文件加载路网" << std::endl;
    std::cout << " 3. 自动加载历史数据" << std::endl;
    std::cout << " 0. 返回主菜单" << std::endl;

    const int choice = get_menu_choice(0, 3);
    if (choice == 0) {
        return;
    }

    Formatter::print_warning("文件管理入口已保留，待服务层实现后接入。");
    Formatter::pause();
}

void MenuSystem::menu_help() {
    Formatter::print_sub_title("帮助 / 关于");
    std::cout << "本系统按课程设计要求预留 9 项主菜单：" << std::endl;
    std::cout << "路网编辑、图遍历、最短路径、最小生成树、拓扑排序、"
              << "拥堵模拟、结构对比、数据文件管理、帮助/关于。"
              << std::endl;
    std::cout << "当前版本完成了 UI 层骨架、统一输出格式化和输入校验。"
              << std::endl;
    Formatter::pause();
}

void MenuSystem::init_network() {
    network = nullptr;
    simulator = nullptr;
    comparator = nullptr;
}

void MenuSystem::load_default_data() {
    Formatter::print_info("默认示例数据加载入口已保留，暂未接入具体数据源。");
}