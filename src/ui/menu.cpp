/*
 * 模块名称  : UI——菜单系统
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 多级菜单框架——主菜单 + 各功能模块子菜单。
 *             负责用户交互流程控制与功能调度。
 */

#include "menu.h"

#include "formatter.h"
#include "validator.h"

#include "../common/types.h"

#include <iostream>
#include <string>

static void print_service_unavailable(const char *module_name)
{
    Formatter::print_warning(module_name);
    Formatter::print_info("当前仓库还缺少 services/ 和图算法层对应的 .cpp 实现，UI 已保留入口。");
}

static void print_return_hint()
{
    std::cout << " 0. 返回主菜单" << std::endl;
}

MenuSystem::MenuSystem()
    : network(nullptr),
      simulator(nullptr),
      comparator(nullptr),
      is_running(false)
{
}

MenuSystem::~MenuSystem()
{
    /*
     * 这里暂不 delete 三个服务对象：当前项目只有服务层头文件，没有对应 .cpp。
     * 等 RoadNetwork / CongestionSimulator / StructureComparator 实现后，
     * 再在 init_network() 中创建对象，并在析构函数中释放。
     */
    network = nullptr;
    simulator = nullptr;
    comparator = nullptr;
}

void MenuSystem::run()
{
    is_running = true;
    init_network();
    load_default_data();

    while (is_running) {
        show_main_menu();
        const int choice = get_menu_choice(0, 9);
        dispatch_choice(choice);
    }
}

void MenuSystem::show_welcome() const
{
    Formatter::print_title("城市交通路网分析系统");
    Formatter::print_info("系统采用邻接矩阵和邻接表两种结构，面向城市路网建模与算法分析。");
    Formatter::print_info("当前 UI 层已完成菜单、格式化输出和安全输入校验。");
}

void MenuSystem::show_main_menu() const
{
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

int MenuSystem::get_menu_choice(int min, int max) const
{
    return Validator::read_int_safe("请选择功能编号: ", min, max);
}

void MenuSystem::dispatch_choice(int choice)
{
    switch (choice) {
    case 0:
        if (Validator::read_confirm("确认退出系统？(Y/N): ")) {
            is_running = false;
            Formatter::print_success("系统已退出。");
        }
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

void MenuSystem::menu_network_edit()
{
    while (true) {
        Formatter::print_sub_title("路网编辑");
        std::cout << " 1. 新增城市" << std::endl;
        std::cout << " 2. 删除城市" << std::endl;
        std::cout << " 3. 新增道路" << std::endl;
        std::cout << " 4. 删除道路" << std::endl;
        std::cout << " 5. 修改道路权值" << std::endl;
        std::cout << " 6. 查看路网总览" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 6);
        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            const int city_id = Validator::read_int_safe("城市编号: ", 1, MAX_CITY_COUNT);
            std::string city_name;
            Validator::read_str_safe("城市名称: ", city_name, MAX_CITY_NAME - 1);
            if (!Validator::validate_city_id(city_id) ||
                !Validator::is_valid_city_name(city_name)) {
                Formatter::pause();
                continue;
            }
        } else if (choice == 2) {
            Validator::read_int_safe("要删除的城市编号: ", 1, MAX_CITY_COUNT);
        } else if (choice == 3) {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            const int weight = Validator::read_int_safe("道路权值: ", 1, INF_WEIGHT - 1);
            if (!Validator::validate_no_self_loop(from, to) ||
                !Validator::validate_weight(weight)) {
                Formatter::pause();
                continue;
            }
        } else if (choice == 4) {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            if (!Validator::validate_no_self_loop(from, to)) {
                Formatter::pause();
                continue;
            }
        } else if (choice == 5) {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            const int weight = Validator::read_int_safe("新的道路权值: ", 1, INF_WEIGHT - 1);
            if (!Validator::validate_no_self_loop(from, to) ||
                !Validator::validate_weight(weight)) {
                Formatter::pause();
                continue;
            }
        }

        print_service_unavailable("路网服务尚未接入，暂不能真正修改图数据。");
        Formatter::pause();
    }
}

void MenuSystem::menu_traversal()
{
    while (true) {
        Formatter::print_sub_title("图遍历");
        std::cout << " 1. DFS 深度优先遍历" << std::endl;
        std::cout << " 2. BFS 广度优先遍历" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 2);
        if (choice == 0) {
            return;
        }

        Validator::read_int_safe("遍历起点城市编号: ", 1, MAX_CITY_COUNT);
        if (choice == 1) {
            print_service_unavailable("DFS 遍历算法尚未接入。");
        } else {
            print_service_unavailable("BFS 遍历算法尚未接入。");
        }
        Formatter::pause();
    }
}

void MenuSystem::menu_shortest_path()
{
    while (true) {
        Formatter::print_sub_title("最短路径");
        std::cout << " 1. Dijkstra 单源最短路径" << std::endl;
        std::cout << " 2. Floyd 多源最短路径" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 2);
        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            print_service_unavailable("Dijkstra 算法尚未接入。");
        } else {
            print_service_unavailable("Floyd 算法尚未接入。");
        }
        Formatter::pause();
    }
}

void MenuSystem::menu_spanning_tree()
{
    while (true) {
        Formatter::print_sub_title("最小生成树");
        std::cout << " 1. Prim 算法" << std::endl;
        std::cout << " 2. Kruskal 算法" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 2);
        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            print_service_unavailable("Prim 最小生成树算法尚未接入。");
        } else {
            print_service_unavailable("Kruskal 最小生成树算法尚未接入。");
        }
        Formatter::pause();
    }
}

void MenuSystem::menu_topological_sort()
{
    Formatter::print_sub_title("拓扑排序");
    print_service_unavailable("拓扑排序入口已保留，等待 topological.cpp 实现后接入。");
    Formatter::pause();
}

void MenuSystem::menu_congestion()
{
    while (true) {
        Formatter::print_sub_title("拥堵模拟");
        std::cout << " 1. 修改道路权值" << std::endl;
        std::cout << " 2. 恢复全部修改" << std::endl;
        std::cout << " 3. 查看修改记录" << std::endl;
        std::cout << " 4. 拥堵前后对比分析" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 4);
        if (choice == 0) {
            return;
        }

        if (choice == 1) {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            const int weight = Validator::read_int_safe("拥堵后的道路权值: ", 1, INF_WEIGHT - 1);
            if (!Validator::validate_no_self_loop(from, to) ||
                !Validator::validate_weight(weight)) {
                Formatter::pause();
                continue;
            }
        } else if (choice == 4) {
            Validator::read_int_safe("分析起点城市编号: ", 1, MAX_CITY_COUNT);
        }

        print_service_unavailable("拥堵模拟器尚未接入。");
        Formatter::pause();
    }
}

void MenuSystem::menu_comparator()
{
    Formatter::print_sub_title("结构性能对比");
    print_service_unavailable("结构性能对比器尚未接入。");
    Formatter::pause();
}

void MenuSystem::menu_file_manage()
{
    while (true) {
        Formatter::print_sub_title("数据文件管理");
        std::cout << " 1. 保存路网到文件" << std::endl;
        std::cout << " 2. 从文件加载路网" << std::endl;
        std::cout << " 3. 设置默认文件路径" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 3);
        if (choice == 0) {
            return;
        }

        if (choice == 1 || choice == 2 || choice == 3) {
            std::string path;
            Validator::read_str_safe("TXT 文件路径: ", path, 255);
            if (!Validator::is_valid_file_path(path)) {
                Formatter::print_error("文件路径非法，路径必须指向 .txt 文件。");
                Formatter::pause();
                continue;
            }
        }

        print_service_unavailable("文件读写服务尚未接入。");
        Formatter::pause();
    }
}

void MenuSystem::menu_help()
{
    Formatter::print_sub_title("帮助 / 关于");
    std::cout << "本系统面向城市交通路网建模，主功能包括：" << std::endl;
    std::cout << " 1. 城市和道路的增删改查" << std::endl;
    std::cout << " 2. DFS / BFS 图遍历" << std::endl;
    std::cout << " 3. Dijkstra / Floyd 最短路径分析" << std::endl;
    std::cout << " 4. Prim / Kruskal 最小生成树" << std::endl;
    std::cout << " 5. 拓扑排序、拥堵模拟、结构性能对比和文件管理" << std::endl;
    std::cout << std::endl;
    std::cout << "当前重写内容集中在 UI 层：菜单流程、输入保护、输出格式。"
              << std::endl;
    Formatter::pause();
}

void MenuSystem::init_network()
{
    network = nullptr;
    simulator = nullptr;
    comparator = nullptr;
}

void MenuSystem::load_default_data()
{
    Formatter::print_info("默认示例数据加载入口已保留。服务层实现后可在此初始化路网。");
}
