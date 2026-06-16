/*
 * 模块名称  : UI——菜单系统
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 多级菜单框架——主菜单 + 各功能模块子菜单。
 *             负责用户交互流程控制与功能调度。
 */

#include "menu.h"

#include "formatter.h"
#include "../services/file_io.h"
#include "validator.h"

#include "../common/types.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{
struct DemoCity
{
    int id;
    const char *name;
};

struct DemoRoad
{
    int from;
    int to;
    int weight;
};

struct CongestionState
{
    bool active;
    int from;
    int to;
    int original_weight;
    int congested_weight;
};

bool try_restore_congestion(MenuSystem *menu);
} // namespace

static void print_service_unavailable(const char *module_name)
{
    Formatter::print_warning(module_name);
    Formatter::print_info("当前仓库还缺少算法层对应的 .cpp 实现，UI 已保留入口。");
}

static void print_return_hint()
{
    std::cout << " 0. 返回主菜单" << std::endl;
}

static int clear_graph(GraphBase *graph)
{
    if (graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int *vertex_ids = nullptr;
    int vertex_count = 0;
    int ret = graph->get_all_vertex_ids(&vertex_ids, &vertex_count);
    if (ret != SUCCESS) {
        return ret;
    }

    for (int i = 0; i < vertex_count; ++i) {
        graph->remove_vertex(vertex_ids[i]);
    }

    delete[] vertex_ids;
    return SUCCESS;
}

MenuSystem::MenuSystem()
    : network(nullptr),
      simulator(nullptr),
      comparator(nullptr),
            is_running(false),
            congestion_active(false),
            congestion_from(INVALID_ID),
            congestion_to(INVALID_ID),
            congestion_original_weight(INF_WEIGHT),
            congestion_congested_weight(INF_WEIGHT)
{
}

MenuSystem::~MenuSystem()
{
        reset_congestion_state();

        delete comparator;
        delete simulator;
        delete network;

        comparator = nullptr;
        simulator = nullptr;
    network = nullptr;
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
    Formatter::print_info("当前 UI 层已接入路网管理、文件读写、拥堵模拟和性能对比。");
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
    if (network == nullptr) {
        Formatter::print_error("路网尚未初始化。");
        Formatter::pause();
        return;
    }

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

            reset_congestion_state();
            const int ret = network->add_city(city_id, city_name.c_str());
            if (ret == SUCCESS) {
                Formatter::print_success("城市添加成功。");
            } else {
                Formatter::print_error("城市添加失败。");
            }
        } else if (choice == 2) {
            const int city_id = Validator::read_int_safe("要删除的城市编号: ", 1, MAX_CITY_COUNT);
            reset_congestion_state();
            const int ret = network->remove_city(city_id);
            if (ret == SUCCESS) {
                Formatter::print_success("城市删除成功。");
            } else {
                Formatter::print_error("城市删除失败。");
            }
        } else if (choice == 3) {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            const int weight = Validator::read_int_safe("道路权值: ", 1, INF_WEIGHT - 1);
            if (!Validator::validate_no_self_loop(from, to) ||
                !Validator::validate_weight(weight)) {
                Formatter::pause();
                continue;
            }

            reset_congestion_state();
            const int ret = network->add_road(from, to, weight);
            if (ret == SUCCESS) {
                Formatter::print_success("道路添加成功。");
            } else {
                Formatter::print_error("道路添加失败。");
            }
        } else if (choice == 4) {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            if (!Validator::validate_no_self_loop(from, to)) {
                Formatter::pause();
                continue;
            }

            reset_congestion_state();
            const int ret = network->remove_road(from, to);
            if (ret == SUCCESS) {
                Formatter::print_success("道路删除成功。");
            } else {
                Formatter::print_error("道路删除失败。");
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

            reset_congestion_state();
            const int ret = network->update_road_weight(from, to, weight);
            if (ret == SUCCESS) {
                Formatter::print_success("道路权值修改成功。");
            } else {
                Formatter::print_error("道路权值修改失败。");
            }
        } else if (choice == 6) {
            network->print_network_overview();
        }
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
    if (network == nullptr || simulator == nullptr) {
        Formatter::print_error("拥堵模拟器尚未初始化。");
        Formatter::pause();
        return;
    }

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

            if (congestion_active) {
                reset_congestion_state();
            }

            int original_weight = 0;
            GraphBase *matrix_graph = network->get_graph(STORAGE_MATRIX);
            GraphBase *list_graph = network->get_graph(STORAGE_LIST);
            if (matrix_graph == nullptr || list_graph == nullptr) {
                Formatter::print_error("底层图结构不可用。");
                Formatter::pause();
                continue;
            }

            if (matrix_graph->get_edge_weight(from, to, &original_weight) != SUCCESS) {
                Formatter::print_error("指定道路不存在，无法设置拥堵。");
                Formatter::pause();
                continue;
            }

            if (simulator->set_congestion(from, to, weight) != SUCCESS) {
                Formatter::print_error("设置拥堵失败。");
                Formatter::pause();
                continue;
            }

            if (list_graph->update_edge_weight(from, to, weight) != SUCCESS) {
                simulator->restore_all();
                Formatter::print_error("同步邻接表失败，已回滚。");
                Formatter::pause();
                continue;
            }

            congestion_active = true;
            congestion_from = from;
            congestion_to = to;
            congestion_original_weight = original_weight;
            congestion_congested_weight = weight;
            Formatter::print_success("拥堵权值修改成功。");
        } else if (choice == 4) {
            const int start_city = Validator::read_int_safe("分析起点城市编号: ", 1, MAX_CITY_COUNT);
            simulator->print_comparison_report(start_city);
        } else if (choice == 2) {
            reset_congestion_state();
            Formatter::print_success("已恢复全部拥堵修改。");
        } else if (choice == 3) {
            if (congestion_active) {
                std::cout << "当前拥堵道路: " << congestion_from
                          << " -> " << congestion_to
                          << "，原始权值 " << congestion_original_weight
                          << "，拥堵后权值 " << congestion_congested_weight
                          << std::endl;
            } else {
                std::cout << "当前没有拥堵修改记录。" << std::endl;
            }
        }
        Formatter::pause();
    }
}

void MenuSystem::menu_comparator()
{
    if (comparator == nullptr) {
        Formatter::print_error("性能对比器尚未初始化。");
        Formatter::pause();
        return;
    }

    Formatter::print_sub_title("结构性能对比");
    comparator->run_full_comparison();
    Formatter::pause();
}

void MenuSystem::menu_file_manage()
{
    if (network == nullptr) {
        Formatter::print_error("路网尚未初始化。");
        Formatter::pause();
        return;
    }

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

        std::string path;
        if (choice == 1 || choice == 2 || choice == 3) {
            Validator::read_str_safe("TXT 文件路径: ", path, 255);
            if (!Validator::is_valid_file_path(path)) {
                Formatter::print_error("文件路径非法，路径必须指向 .txt 文件。");
                Formatter::pause();
                continue;
            }
        }

        GraphBase *matrix_graph = network->get_graph(STORAGE_MATRIX);
        GraphBase *list_graph = network->get_graph(STORAGE_LIST);

        if (choice == 1) {
            const int ret = FileManager::save_to_file(matrix_graph, path.c_str());
            if (ret == SUCCESS) {
                Formatter::print_success("路网已保存到文件。");
            } else {
                Formatter::print_error("保存文件失败。");
            }
        } else if (choice == 2) {
            reset_congestion_state();
            int ret = FileManager::load_from_file(matrix_graph, path.c_str());
            if (ret == SUCCESS) {
                ret = FileManager::load_from_file(list_graph, path.c_str());
            }

            if (ret == SUCCESS) {
                Formatter::print_success("路网已从文件加载。");
            } else {
                Formatter::print_error("加载文件失败。");
            }
        } else if (choice == 3) {
            FileManager::set_default_path(path.c_str());
            Formatter::print_success("默认文件路径已更新。");
        }
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
    delete comparator;
    delete simulator;
    delete network;

    network = new RoadNetwork(MAX_CITY_COUNT, GRAPH_UNDIRECTED);
    simulator = new CongestionSimulator(network->get_graph(STORAGE_MATRIX), MAX_CITY_COUNT);
    comparator = new StructureComparator(network->get_graph(STORAGE_MATRIX),
                                         network->get_graph(STORAGE_LIST));
}

void MenuSystem::load_default_data()
{
    if (network == nullptr) {
        Formatter::print_error("路网初始化失败。");
        return;
    }

    reset_congestion_state();

    GraphBase *matrix_graph = network->get_graph(STORAGE_MATRIX);
    GraphBase *list_graph = network->get_graph(STORAGE_LIST);
    if (matrix_graph == nullptr || list_graph == nullptr) {
        Formatter::print_error("底层图结构不可用。");
        return;
    }

    if (FileManager::data_file_exists()) {
        int ret = FileManager::auto_load(matrix_graph);
        if (ret == SUCCESS) {
            ret = FileManager::auto_load(list_graph);
        }

        if (ret == SUCCESS) {
            Formatter::print_success("历史路网数据加载成功。");
            return;
        }

        Formatter::print_warning("历史数据加载失败，已切换为内置示例路网。");
        clear_graph(matrix_graph);
        clear_graph(list_graph);
    }

    const DemoCity demo_cities[] = {
        {1, "City1"},
        {2, "City2"},
        {3, "City3"},
        {4, "City4"},
        {5, "City5"},
    };

    const DemoRoad demo_roads[] = {
        {1, 2, 10},
        {1, 3, 18},
        {2, 3, 6},
        {2, 4, 14},
        {3, 5, 7},
        {4, 5, 9},
    };

    for (const DemoCity &city : demo_cities) {
        network->add_city(city.id, city.name);
    }

    for (const DemoRoad &road : demo_roads) {
        network->add_road(road.from, road.to, road.weight);
    }

    Formatter::print_info("已加载内置示例路网。");
}

void MenuSystem::reset_congestion_state()
{
    if (!congestion_active || network == nullptr || simulator == nullptr) {
        congestion_active = false;
        congestion_from = INVALID_ID;
        congestion_to = INVALID_ID;
        congestion_original_weight = INF_WEIGHT;
        congestion_congested_weight = INF_WEIGHT;
        return;
    }

    GraphBase *matrix_graph = network->get_graph(STORAGE_MATRIX);
    GraphBase *list_graph = network->get_graph(STORAGE_LIST);
    if (matrix_graph != nullptr) {
        matrix_graph->update_edge_weight(congestion_from, congestion_to, congestion_original_weight);
    }

    if (list_graph != nullptr) {
        list_graph->update_edge_weight(congestion_from, congestion_to, congestion_original_weight);
    }

    simulator->restore_all();

    congestion_active = false;
    congestion_from = INVALID_ID;
    congestion_to = INVALID_ID;
    congestion_original_weight = INF_WEIGHT;
    congestion_congested_weight = INF_WEIGHT;
}
