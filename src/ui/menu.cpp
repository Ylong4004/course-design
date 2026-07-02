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
#include "../algorithms/traversal.h"
#include "../algorithms/shortest_path.h"
#include "../algorithms/spanning_tree.h"
#include "../algorithms/topological.h"

#include "../common/types.h"
#include "../common/defines.h"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <cstring>

/* 列出 data/ 下所有 .json 文件 */
/**
 * @brief 列出 data/ 目录下所有 .json 路网文件
 * @return 文件路径字符串列表
 */
static std::vector<std::string> list_network_files()
{
    std::vector<std::string> files;
    FileManager::list_data_files(files, true);
    return files;
}

/**
 * @brief 打印"返回主菜单"提示
 */
static void print_return_hint()
{
    std::cout << " 0. 返回主菜单" << std::endl;
}

/**
 * @brief 清空图中所有顶点和边
 * @param graph 图指针
 * @return SUCCESS 或错误码
 */
static int clear_graph(GraphBase *graph)
{
    if (graph == nullptr)
    {
        return ERR_INVALID_INPUT;
    }

    int *vertex_ids = nullptr;
    int vertex_count = 0;
    int ret = graph->get_all_vertex_ids(&vertex_ids, &vertex_count);
    if (ret != SUCCESS)
    {
        return ret;
    }

    for (int i = 0; i < vertex_count; ++i)
    {
        graph->remove_vertex(vertex_ids[i]);
    }

    delete[] vertex_ids;
    return SUCCESS;
}

/**
 * @brief 构造函数，初始化菜单系统成员变量为默认值
 */
MenuSystem::MenuSystem()
    : network(nullptr),
      simulator(nullptr),
      comparator(nullptr),
      is_running(false),
      congestion_active(false),
      congestion_from(INVALID_ID),
      congestion_to(INVALID_ID),
      congestion_original_weight(INF_WEIGHT),
      congestion_congested_weight(INF_WEIGHT),
      switch_to_cli(false)
{
    std::strcpy(current_file_path, "./data/default.json");
}

/**
 * @brief 析构函数，重置拥堵状态并释放所有资源
 */
MenuSystem::~MenuSystem()
{
    reset_congestion_state();

    safe_delete(comparator);
    safe_delete(simulator);
    safe_delete(network);
}

/**
 * @brief 启动系统主循环：初始化路网 -> 加载默认数据 -> 进入菜单分发循环
 */
void MenuSystem::run()
{
    is_running = true;
    init_network();
    load_default_data();

    while (is_running)
    {
        show_main_menu();
        const int choice = get_menu_choice(0, 10);
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
    std::cout << " 10. 命令行模式" << std::endl;
    std::cout << " 0. 退出系统" << std::endl;
}

int MenuSystem::get_menu_choice(int min, int max) const
{
    return Validator::read_int_safe("请选择功能编号: ", min, max);
}

void MenuSystem::dispatch_choice(int choice)
{
    switch (choice)
    {
    case 0:
        if (Validator::read_confirm("确认退出系统？(Y/N): "))
        {
            /* 退出前自动保存到当前路网文件 */
            GraphBase *g = network->get_graph(STORAGE_MATRIX);
            if (g != nullptr && g->get_vertex_count() > 0)
            {
                FileManager::save_to_file(g, current_file_path);
                Formatter::print_info("路网已自动保存。");
            }
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
    case 10:
        if (Validator::read_confirm("确认切换到命令行模式？(Y/N): "))
        {
            switch_to_cli = true;
            is_running = false;
            Formatter::print_info("正在切换到命令行模式...");
        }
        break;
    default:
        Formatter::print_warning("无效菜单项，请重新选择。");
        break;
    }
}

void MenuSystem::menu_network_edit()
{
    if (network == nullptr)
    {
        Formatter::print_error("路网尚未初始化。");
        Formatter::pause();
        return;
    }

    while (true)
    {
        Formatter::print_sub_title("路网编辑");
        std::cout << " 1. 新增城市" << std::endl;
        std::cout << " 2. 删除城市" << std::endl;
        std::cout << " 3. 新增道路" << std::endl;
        std::cout << " 4. 删除道路" << std::endl;
        std::cout << " 5. 修改道路权值" << std::endl;
        std::cout << " 6. 查看路网总览" << std::endl;
        std::cout << " 7. 查看全部节点和路径" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 7);
        if (choice == 0)
        {
            return;
        }

        if (choice == 1)
        {
            const int city_id = Validator::read_int_safe("城市编号: ", 1, MAX_CITY_COUNT);
            std::string city_name;
            Validator::read_str_safe("城市名称: ", city_name, MAX_CITY_NAME - 1);
            if (!Validator::validate_city_id(city_id) ||
                !Validator::is_valid_city_name(city_name))
            {
                Formatter::pause();
                continue;
            }

            reset_congestion_state();
            const int ret = network->add_city(city_id, city_name.c_str());
            if (ret == SUCCESS)
            {
                Formatter::print_success("城市添加成功。");
            }
            else if (ret == ERR_CITY_DUPLICATE)
            {
                Formatter::print_error("城市编号已存在，请更换编号。");
            }
            else if (ret == ERR_GRAPH_FULL)
            {
                Formatter::print_error("城市数量已达上限，无法添加。");
            }
            else
            {
                std::cerr << "[错误] 城市添加失败（错误码: " << ret << "）。" << std::endl;
            }
        }
        else if (choice == 2)
        {
            const int city_id = Validator::read_int_safe("要删除的城市编号: ", 1, MAX_CITY_COUNT);
            reset_congestion_state();
            const int ret = network->remove_city(city_id);
            if (ret == SUCCESS)
            {
                Formatter::print_success("城市删除成功。");
            }
            else if (ret == ERR_CITY_NOT_FOUND)
            {
                Formatter::print_error("城市编号不存在，无法删除。");
            }
            else
            {
                std::cerr << "[错误] 城市删除失败（错误码: " << ret << "）。" << std::endl;
            }
        }
        else if (choice == 3)
        {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            const int weight = Validator::read_int_safe("道路权值: ", 1, INF_WEIGHT - 1);
            if (!Validator::validate_no_self_loop(from, to) ||
                !Validator::validate_weight(weight))
            {
                Formatter::pause();
                continue;
            }

            reset_congestion_state();
            const int ret = network->add_road(from, to, weight);
            if (ret == SUCCESS)
            {
                Formatter::print_success("道路添加成功。");
            }
            else if (ret == ERR_CITY_NOT_FOUND)
            {
                Formatter::print_error("起点或终点城市不存在。");
            }
            else if (ret == ERR_ROAD_EXISTS)
            {
                Formatter::print_error("该道路已存在，请删除后重新添加。");
            }
            else if (ret == ERR_SELF_LOOP)
            {
                Formatter::print_error("不能添加自环道路（起终点相同）。");
            }
            else
            {
                std::cerr << "[错误] 道路添加失败（错误码: " << ret << "）。" << std::endl;
            }
        }
        else if (choice == 4)
        {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            if (!Validator::validate_no_self_loop(from, to))
            {
                Formatter::pause();
                continue;
            }

            reset_congestion_state();
            const int ret = network->remove_road(from, to);
            if (ret == SUCCESS)
            {
                Formatter::print_success("道路删除成功。");
            }
            else if (ret == ERR_ROAD_NOT_FOUND)
            {
                Formatter::print_error("该道路不存在，无法删除。");
            }
            else
            {
                std::cerr << "[错误] 道路删除失败（错误码: " << ret << "）。" << std::endl;
            }
        }
        else if (choice == 5)
        {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            const int weight = Validator::read_int_safe("新的道路权值: ", 1, INF_WEIGHT - 1);
            if (!Validator::validate_no_self_loop(from, to) ||
                !Validator::validate_weight(weight))
            {
                Formatter::pause();
                continue;
            }

            reset_congestion_state();
            const int ret = network->update_road_weight(from, to, weight);
            if (ret == SUCCESS)
            {
                Formatter::print_success("道路权值修改成功。");
            }
            else if (ret == ERR_ROAD_NOT_FOUND)
            {
                Formatter::print_error("该道路不存在，无法修改权值。");
            }
            else
            {
                std::cerr << "[错误] 道路权值修改失败（错误码: " << ret << "）。" << std::endl;
            }
        }
        else if (choice == 6)
        {
            network->print_network_overview();
        }
        else if (choice == 7)
        {
            network->print_network_detail();
        }
        Formatter::pause();
    }
}

void MenuSystem::menu_traversal()
{
    while (true)
    {
        Formatter::print_sub_title("图遍历");
        std::cout << " 1. DFS 深度优先遍历" << std::endl;
        std::cout << " 2. BFS 广度优先遍历" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 2);
        if (choice == 0)
        {
            return;
        }

        GraphBase *graph = network->get_graph(STORAGE_MATRIX);
        int start = Validator::read_int_safe("遍历起点城市编号: ", 1, MAX_CITY_COUNT);
        int *seq = nullptr;
        int len = 0;
        if (choice == 1)
        {
            traverse_dfs(graph, start, &seq, &len);
            print_traversal_sequence(graph, seq, len, "DFS");
        }
        else
        {
            traverse_bfs(graph, start, &seq, &len);
            print_traversal_sequence(graph, seq, len, "BFS");
        }
        delete[] seq;
        Formatter::pause();
    }
}

void MenuSystem::menu_shortest_path()
{
    while (true)
    {
        Formatter::print_sub_title("最短路径");
        std::cout << " 1. Dijkstra 单源最短路径" << std::endl;
        std::cout << " 2. Floyd 多源最短路径" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 2);
        if (choice == 0)
        {
            return;
        }

        GraphBase *graph = network->get_graph(STORAGE_MATRIX);
        if (choice == 1)
        {
            int start = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            int vc = graph->get_vertex_count();
            int *dist = new int[vc];
            int *prev = new int[vc];
            run_dijkstra(graph, start, dist, prev);
            print_dijkstra_result(graph, start, dist, prev);
            delete[] dist;
            delete[] prev;
        }
        else
        {
            int vc = 0;
            int **dist = nullptr;
            int **next = nullptr;
            run_floyd(graph, &dist, &next, &vc);
            print_floyd_table(graph, dist, next, vc);
            for (int i = 0; i < vc; ++i)
            {
                delete[] dist[i];
                delete[] next[i];
            }
            delete[] dist;
            delete[] next;
        }
        Formatter::pause();
    }
}

void MenuSystem::menu_spanning_tree()
{
    while (true)
    {
        Formatter::print_sub_title("最小生成树");
        std::cout << " 1. Prim 算法" << std::endl;
        std::cout << " 2. Kruskal 算法" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 2);
        if (choice == 0)
        {
            return;
        }

        GraphBase *graph = network->get_graph(STORAGE_MATRIX);
        MSTResult_t mst;
        if (choice == 1)
        {
            build_mst_prim(graph, &mst);
            print_mst_result("Prim", &mst);
        }
        else
        {
            build_mst_kruskal(graph, &mst);
            print_mst_result("Kruskal", &mst);
        }
        free_mst_result(&mst);
        Formatter::pause();
    }
}

void MenuSystem::menu_topological_sort()
{
    Formatter::print_sub_title("拓扑排序");
    GraphBase *graph = network->get_graph(STORAGE_MATRIX);

    if (graph->get_graph_type() != GRAPH_DIRECTED)
    {
        Formatter::print_error("拓扑排序仅适用于有向图，"
                               "当前路网为无向图，无法执行此操作。");
        Formatter::print_info("提示：请在路网编辑中构建有向图后重试。");
        Formatter::pause();
        return;
    }

    int *seq = nullptr;
    int len = 0;
    bool has_cycle = false;
    int ret = run_topological_sort(graph, &seq, &len, &has_cycle);
    if (ret != SUCCESS)
    {
        Formatter::print_error("拓扑排序执行失败。");
        delete[] seq;
        Formatter::pause();
        return;
    }
    print_topo_result(graph, seq, len, has_cycle);
    delete[] seq;
    Formatter::pause();
}

void MenuSystem::menu_congestion()
{
    if (network == nullptr || simulator == nullptr)
    {
        Formatter::print_error("拥堵模拟器尚未初始化。");
        Formatter::pause();
        return;
    }

    while (true)
    {
        Formatter::print_sub_title("拥堵模拟");
        std::cout << " 1. 修改道路权值" << std::endl;
        std::cout << " 2. 恢复全部修改" << std::endl;
        std::cout << " 3. 查看修改记录" << std::endl;
        std::cout << " 4. 拥堵前后对比分析" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 4);
        if (choice == 0)
        {
            return;
        }

        if (choice == 1)
        {
            const int from = Validator::read_int_safe("起点城市编号: ", 1, MAX_CITY_COUNT);
            const int to = Validator::read_int_safe("终点城市编号: ", 1, MAX_CITY_COUNT);
            const int weight = Validator::read_int_safe("拥堵后的道路权值: ", 1, INF_WEIGHT - 1);
            if (!Validator::validate_no_self_loop(from, to) ||
                !Validator::validate_weight(weight))
            {
                Formatter::pause();
                continue;
            }

            if (congestion_active)
            {
                reset_congestion_state();
            }

            int original_weight = 0;
            GraphBase *matrix_graph = network->get_graph(STORAGE_MATRIX);
            GraphBase *list_graph = network->get_graph(STORAGE_LIST);
            if (matrix_graph == nullptr || list_graph == nullptr)
            {
                Formatter::print_error("底层图结构不可用。");
                Formatter::pause();
                continue;
            }

            if (matrix_graph->get_edge_weight(from, to, &original_weight) != SUCCESS)
            {
                Formatter::print_error("指定道路不存在，无法设置拥堵。");
                Formatter::pause();
                continue;
            }

            if (simulator->set_congestion(from, to, weight) != SUCCESS)
            {
                Formatter::print_error("设置拥堵失败。");
                Formatter::pause();
                continue;
            }

            if (list_graph->update_edge_weight(from, to, weight) != SUCCESS)
            {
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
        }
        else if (choice == 4)
        {
            const int start_city = Validator::read_int_safe("分析起点城市编号: ", 1, MAX_CITY_COUNT);
            simulator->print_comparison_report(start_city);
        }
        else if (choice == 2)
        {
            reset_congestion_state();
            Formatter::print_success("已恢复全部拥堵修改。");
        }
        else if (choice == 3)
        {
            if (congestion_active)
            {
                std::cout << "当前拥堵道路: " << congestion_from
                          << " -> " << congestion_to
                          << "，原始权值 " << congestion_original_weight
                          << "，拥堵后权值 " << congestion_congested_weight
                          << std::endl;
            }
            else
            {
                std::cout << "当前没有拥堵修改记录。" << std::endl;
            }
        }
        Formatter::pause();
    }
}

void MenuSystem::menu_comparator()
{
    if (comparator == nullptr)
    {
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
    if (network == nullptr)
    {
        Formatter::print_error("路网尚未初始化。");
        Formatter::pause();
        return;
    }

    while (true)
    {
        Formatter::print_sub_title("数据文件管理");
        std::cout << " 1. 保存路网到文件" << std::endl;
        std::cout << " 2. 从文件加载路网" << std::endl;
        std::cout << " 3. 切换路网（从 data/ 选择）" << std::endl;
        std::cout << " 4. 设置默认文件路径" << std::endl;
        std::cout << " 5. 新建路网" << std::endl;
        print_return_hint();

        const int choice = get_menu_choice(0, 5);
        if (choice == 0)
        {
            return;
        }

        GraphBase *matrix_graph = network->get_graph(STORAGE_MATRIX);

        if (choice == 3)
        {
            /* 切换路网：列出 data/ 下所有 .json 文件 */
            std::vector<std::string> files = list_network_files();
            if (files.empty())
            {
                Formatter::print_warning("data/ 目录下没有找到路网文件。");
                Formatter::pause();
                continue;
            }
            Formatter::print_sub_title("可用路网文件");
            for (size_t i = 0; i < files.size(); ++i)
            {
                std::cout << " " << (i + 1) << ". " << files[i] << std::endl;
            }
            std::cout << " 0. 取消" << std::endl;
            int fchoice = get_menu_choice(0, (int)files.size());
            if (fchoice == 0)
            {
                Formatter::pause();
                continue;
            }
            const char *fpath = files[fchoice - 1].c_str();
            int ret = load_network_file(fpath);
            if (ret == SUCCESS)
            {
                Formatter::print_success(("已切换至: " + files[fchoice - 1]).c_str());
            }
            else
            {
                Formatter::print_error("路网加载失败，请检查文件格式。");
            }
            Formatter::pause();
            continue;
        }

        std::string filename;
        std::string fullpath;
        if (choice == 1 || choice == 2 || choice == 5)
        {
            Validator::read_str_safe("文件名（仅 .json 后缀，保存在 data/ 目录）: ", filename, 255);
            if (filename.empty() || filename.size() < 6 ||
                filename.compare(filename.size() - 5, 5, ".json") != 0)
            {
                filename += ".json";
            }
            fullpath = "./data/" + filename;
        }
        else if (choice == 4)
        {
            Validator::read_str_safe("默认文件路径: ", filename, 255);
            fullpath = filename;
        }

        if (choice == 1)
        {
            const int ret = FileManager::save_to_file(matrix_graph, fullpath.c_str());
            if (ret == SUCCESS)
            {
                std::strcpy(current_file_path, fullpath.c_str());
                Formatter::print_success(("路网已保存到 " + fullpath).c_str());
            }
            else
            {
                Formatter::print_error("保存文件失败。");
            }
        }
        else if (choice == 2)
        {
            int ret = load_network_file(fullpath.c_str());
            if (ret == SUCCESS)
            {
                Formatter::print_success(("路网已从文件加载: " + fullpath).c_str());
            }
            else
            {
                Formatter::print_error("加载文件失败。");
            }
        }
        else if (choice == 5)
        {
            const int type_value = Validator::read_int_safe("图类型（0=无向图，1=有向图）: ", 0, 1);
            int ret = create_network_file(static_cast<GraphType>(type_value),
                                          fullpath.c_str());
            if (ret == SUCCESS)
            {
                Formatter::print_success(("已新建空路网: " + fullpath).c_str());
            }
            else
            {
                Formatter::print_error("新建路网失败，请检查文件路径。");
            }
        }
        else if (choice == 4)
        {
            FileManager::set_default_path(fullpath.c_str());
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
    safe_delete(comparator);
    safe_delete(simulator);
    safe_delete(network);

    network = new RoadNetwork(MAX_CITY_COUNT, GRAPH_UNDIRECTED);
    simulator = new CongestionSimulator(network->get_graph(STORAGE_MATRIX), MAX_CITY_COUNT);
    comparator = new StructureComparator(network->get_graph(STORAGE_MATRIX), network->get_graph(STORAGE_LIST));
}

void MenuSystem::refresh_runtime_services()
{
    if (network == nullptr)
    {
        return;
    }

    if (simulator != nullptr)
    {
        simulator->set_graph(network->get_graph(STORAGE_MATRIX));
    }
    if (comparator != nullptr)
    {
        comparator->set_graphs(network->get_graph(STORAGE_MATRIX),
                               network->get_graph(STORAGE_LIST));
    }
}

int MenuSystem::load_network_file(const char *path)
{
    if (network == nullptr || path == nullptr || path[0] == '\0')
    {
        return ERR_INVALID_INPUT;
    }

    GraphType file_graph_type = GRAPH_UNDIRECTED;
    int ret = FileManager::detect_graph_type(path, &file_graph_type);
    if (ret != SUCCESS)
    {
        return ret;
    }

    RoadNetwork temp_network(MAX_CITY_COUNT, file_graph_type);
    ret = FileManager::load_from_file(temp_network.get_graph(STORAGE_MATRIX), path);
    if (ret == SUCCESS)
    {
        ret = FileManager::load_from_file(temp_network.get_graph(STORAGE_LIST), path);
    }
    if (ret != SUCCESS)
    {
        return ret;
    }

    reset_congestion_state();
    ret = network->reset(file_graph_type);
    if (ret != SUCCESS)
    {
        return ret;
    }
    refresh_runtime_services();

    ret = FileManager::load_from_file(network->get_graph(STORAGE_MATRIX), path);
    if (ret == SUCCESS)
    {
        ret = FileManager::load_from_file(network->get_graph(STORAGE_LIST), path);
    }
    if (ret == SUCCESS)
    {
        std::strncpy(current_file_path, path, sizeof(current_file_path) - 1);
        current_file_path[sizeof(current_file_path) - 1] = '\0';
    }

    return ret;
}

int MenuSystem::create_network_file(GraphType graph_type, const char *path)
{
    if (network == nullptr || path == nullptr || path[0] == '\0')
    {
        return ERR_INVALID_INPUT;
    }

    RoadNetwork empty_network(MAX_CITY_COUNT, graph_type);
    int ret = FileManager::save_to_file(empty_network.get_graph(STORAGE_MATRIX), path);
    if (ret != SUCCESS)
    {
        return ret;
    }

    reset_congestion_state();
    ret = network->reset(graph_type);
    if (ret != SUCCESS)
    {
        return ret;
    }
    refresh_runtime_services();

    std::strncpy(current_file_path, path, sizeof(current_file_path) - 1);
    current_file_path[sizeof(current_file_path) - 1] = '\0';
    return SUCCESS;
}

void MenuSystem::load_default_data()
{
    if (network == nullptr)
    {
        Formatter::print_error("路网初始化失败。");
        return;
    }

    /* 优先加载 data/default.json */
    const char *default_file = "./data/default.json";
    int ret = load_network_file(default_file);
    if (ret == SUCCESS)
    {
        Formatter::print_success("已加载默认路网数据（data/default.json）。");
        return;
    }

    /* default.json 不存在或加载失败，尝试 data/ 下第一个 .json */
    std::vector<std::string> files = list_network_files();
    for (const auto &f : files)
    {
        if (f.find("default.json") != std::string::npos)
            continue;
        ret = load_network_file(f.c_str());
        if (ret == SUCCESS)
        {
            Formatter::print_success(("已加载路网文件: " + f).c_str());
            return;
        }
    }

    Formatter::print_warning("未找到可用的路网数据文件，请在路网编辑中手动创建城市和道路。");
}

void MenuSystem::reset_congestion_state()
{
    if (!congestion_active || network == nullptr || simulator == nullptr)
    {
        congestion_active = false;
        congestion_from = INVALID_ID;
        congestion_to = INVALID_ID;
        congestion_original_weight = INF_WEIGHT;
        congestion_congested_weight = INF_WEIGHT;
        return;
    }

    GraphBase *matrix_graph = network->get_graph(STORAGE_MATRIX);
    GraphBase *list_graph = network->get_graph(STORAGE_LIST);
    if (matrix_graph != nullptr)
    {
        matrix_graph->update_edge_weight(congestion_from, congestion_to, congestion_original_weight);
    }

    if (list_graph != nullptr)
    {
        list_graph->update_edge_weight(congestion_from, congestion_to, congestion_original_weight);
    }

    simulator->restore_all();

    congestion_active = false;
    congestion_from = INVALID_ID;
    congestion_to = INVALID_ID;
    congestion_original_weight = INF_WEIGHT;
    congestion_congested_weight = INF_WEIGHT;
}
