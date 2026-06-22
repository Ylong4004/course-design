/*
 * 模块名称  : CLI——命令行解析器实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 命令行模式的解析、分派与执行。
 *             直接调用底层算法和服务，绕过菜单层级。
 */

#include "command_parser.h"
#include "../ui/formatter.h"
#include "../ui/validator.h"

#include "../algorithms/traversal.h"
#include "../algorithms/shortest_path.h"
#include "../algorithms/spanning_tree.h"
#include "../algorithms/topological.h"

#include "../common/types.h"
#include "../common/defines.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cstring>

/* ============================================================ */
/*  当前路网文件路径追踪                                          */
/* ============================================================ */

static char g_current_file[256] = "./data/default.txt";

const char *CommandParser::get_current_file()
{
    return g_current_file;
}

void CommandParser::set_current_file(const char *path)
{
    std::strncpy(g_current_file, path, sizeof(g_current_file) - 1);
    g_current_file[sizeof(g_current_file) - 1] = '\0';
}

/* ============================================================ */
/*  工具函数                                                    */
/* ============================================================ */

std::vector<std::string> CommandParser::tokenize(const std::string &line)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(line);
    while (iss >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

bool CommandParser::is_integer(const std::string &s)
{
    if (s.empty())
        return false;
    size_t i = 0;
    if (s[0] == '-' || s[0] == '+')
        i = 1;
    if (i == s.size())
        return false;
    for (; i < s.size(); ++i)
    {
        if (!std::isdigit(static_cast<unsigned char>(s[i])))
            return false;
    }
    return true;
}

int CommandParser::to_int(const std::string &s)
{
    return std::atoi(s.c_str());
}

void CommandParser::print_error(const char *msg)
{
    std::cout << "[错误] " << msg << std::endl;
}

void CommandParser::print_success(const char *msg)
{
    std::cout << "[成功] " << msg << std::endl;
}

/* ============================================================ */
/*  公共接口                                                    */
/* ============================================================ */

bool CommandParser::execute(const std::string &cmd_line,
                            RoadNetwork &network,
                            CongestionSimulator *simulator,
                            StructureComparator *comparator)
{
    std::vector<std::string> argv = tokenize(cmd_line);
    if (argv.empty())
        return true; /* 空行，继续 */
    return dispatch(argv, network, simulator, comparator);
}

void CommandParser::print_help()
{
    Formatter::print_title("命令行模式帮助");

    std::cout << "用法: <命令> [参数1] [参数2] ..." << std::endl;
    std::cout << std::endl;

    const char *cmds[][2] = {
        {"new_city <id> <name>", "添加城市"},
        {"del_city <id>", "删除城市"},
        {"new_road <from> <to> <w>", "添加道路（权值w）"},
        {"del_road <from> <to>", "删除道路"},
        {"update_road <from> <to> <w>", "修改道路权值"},
        {"show_network", "打印路网总览"},
        {"dfs <start_id>", "深度优先遍历"},
        {"bfs <start_id>", "广度优先遍历"},
        {"dijkstra <start> [end]", "单源最短路径（可选终点）"},
        {"floyd", "多源最短路径（全表）"},
        {"prim", "Prim最小生成树"},
        {"kruskal", "Kruskal最小生成树"},
        {"topo", "拓扑排序"},
        {"congest <from> <to> <w>", "设置道路拥堵（新权值w）"},
        {"restore", "恢复所有拥堵道路"},
        {"compare", "邻接矩阵 vs 邻接表 性能对比"},
        {"save [filepath]", "保存路网（默认路径）"},
        {"load [filepath]", "加载路网（默认路径）"},
        {"list", "列出 data/ 下可用路网"},
        {"congest_list", "查看当前拥堵记录"},
        {"congest_report <start>", "拥堵前后对比报告"},
        {"help", "显示本帮助"},
        {"menu", "切换回菜单模式（仅交互模式）"},
        {"exit / quit", "退出程序"},
    };

    for (size_t i = 0; i < sizeof(cmds) / sizeof(cmds[0]); ++i)
    {
        std::cout << "  " << std::left << std::setw(32) << cmds[i][0]
                  << cmds[i][1] << std::endl;
    }
    std::cout << std::endl;
    std::cout << "提示: 参数中的 <...> 为必填，[...] 为选填。" << std::endl;
    Formatter::print_line('-', 60);
}

void CommandParser::run_interactive(RoadNetwork &network,
                                    CongestionSimulator *simulator,
                                    StructureComparator *comparator)
{
    std::cout << std::endl;
    Formatter::print_title("命令行模式");
    std::cout << "输入 'help' 查看命令列表，'menu' 切换回菜单模式，'exit' 退出。"
              << std::endl;
    Formatter::print_line('-', 60);

    std::string line;
    while (true)
    {
        std::cout << "> ";
        if (!std::getline(std::cin, line))
            break;

        if (line.empty())
            continue;

        std::vector<std::string> argv = tokenize(line);
        if (argv.empty())
            continue;

        /* 特殊命令：menu / exit / quit */
        if (argv[0] == "menu")
        {
            std::cout << "切换回菜单模式..." << std::endl;
            break;
        }
        if (argv[0] == "exit" || argv[0] == "quit")
        {
            std::cout << "再见！" << std::endl;
            std::exit(0);
        }

        if (!dispatch(argv, network, simulator, comparator))
        {
            /* dispatch 返回 false 表示 exit */
            std::cout << "再见！" << std::endl;
            std::exit(0);
        }
    }
}

/* ============================================================ */
/*  命令分派                                                    */
/* ============================================================ */

bool CommandParser::dispatch(const std::vector<std::string> &argv,
                             RoadNetwork &network,
                             CongestionSimulator *simulator,
                             StructureComparator *comparator)
{
    const std::string &cmd = argv[0];

    if (cmd == "new_city")
    {
        cmd_new_city(argv, network);
    }
    else if (cmd == "del_city")
    {
        cmd_del_city(argv, network);
    }
    else if (cmd == "new_road")
    {
        cmd_new_road(argv, network);
    }
    else if (cmd == "del_road")
    {
        cmd_del_road(argv, network);
    }
    else if (cmd == "update_road")
    {
        cmd_update_road(argv, network);
    }
    else if (cmd == "show_network")
    {
        cmd_show_network(network);
    }
    else if (cmd == "dfs")
    {
        cmd_dfs(argv, network);
    }
    else if (cmd == "bfs")
    {
        cmd_bfs(argv, network);
    }
    else if (cmd == "dijkstra")
    {
        cmd_dijkstra(argv, network);
    }
    else if (cmd == "floyd")
    {
        cmd_floyd(network);
    }
    else if (cmd == "prim")
    {
        cmd_prim(network);
    }
    else if (cmd == "kruskal")
    {
        cmd_kruskal(network);
    }
    else if (cmd == "topo")
    {
        cmd_topo(network);
    }
    else if (cmd == "congest")
    {
        cmd_congest(argv, network, simulator);
    }
    else if (cmd == "restore")
    {
        cmd_restore(network, simulator);
    }
    else if (cmd == "compare")
    {
        cmd_compare(network, comparator);
    }
    else if (cmd == "save")
    {
        cmd_save(argv, network);
    }
    else if (cmd == "load")
    {
        cmd_load(argv, network);
    }
    else if (cmd == "list")
    {
        cmd_list();
    }
    else if (cmd == "congest_list")
    {
        cmd_congest_list(simulator);
    }
    else if (cmd == "congest_report")
    {
        cmd_congest_report(argv, simulator);
    }
    else if (cmd == "help" || cmd == "?")
    {
        print_help();
    }
    else if (cmd == "exit" || cmd == "quit")
    {
        return false;
    }
    else
    {
        print_error(("未知命令: " + cmd + ", 输入 'help' 查看帮助。").c_str());
    }
    return true;
}

/* ============================================================ */
/*  路网编辑命令                                                */
/* ============================================================ */

void CommandParser::cmd_new_city(const std::vector<std::string> &argv,
                                 RoadNetwork &network)
{
    if (argv.size() < 3)
    {
        print_error("用法: new_city <id> <name>");
        return;
    }
    if (!is_integer(argv[1]))
    {
        print_error("城市ID必须是整数。");
        return;
    }
    int id = to_int(argv[1]);
    std::string name = argv[2];
    if (name.length() >= MAX_CITY_NAME)
    {
        print_error("城市名称过长（最大31字符）。");
        return;
    }
    int rc = network.add_city(id, name.c_str());
    if (rc == SUCCESS)
    {
        print_success(("添加城市 " + name + " (ID=" + argv[1] + ")").c_str());
    }
    else if (rc == ERR_CITY_DUPLICATE)
    {
        print_error("城市ID已存在。");
    }
    else if (rc == ERR_GRAPH_FULL)
    {
        print_error("城市数量已达上限。");
    }
    else
    {
        print_error("添加城市失败。");
    }
}

void CommandParser::cmd_del_city(const std::vector<std::string> &argv,
                                 RoadNetwork &network)
{
    if (argv.size() < 2)
    {
        print_error("用法: del_city <id>");
        return;
    }
    if (!is_integer(argv[1]))
    {
        print_error("城市ID必须是整数。");
        return;
    }
    int id = to_int(argv[1]);
    int rc = network.remove_city(id);
    if (rc == SUCCESS)
    {
        print_success(("删除城市 ID=" + argv[1]).c_str());
    }
    else if (rc == ERR_CITY_NOT_FOUND)
    {
        print_error("城市不存在。");
    }
    else
    {
        print_error("删除城市失败。");
    }
}

void CommandParser::cmd_new_road(const std::vector<std::string> &argv,
                                 RoadNetwork &network)
{
    if (argv.size() < 4)
    {
        print_error("用法: new_road <from> <to> <weight>");
        return;
    }
    if (!is_integer(argv[1]) || !is_integer(argv[2]) || !is_integer(argv[3]))
    {
        print_error("参数必须是整数。");
        return;
    }
    int from = to_int(argv[1]);
    int to = to_int(argv[2]);
    int w = to_int(argv[3]);
    if (w <= 0)
    {
        print_error("道路权值必须为正整数。");
        return;
    }
    int rc = network.add_road(from, to, w);
    if (rc == SUCCESS)
    {
        print_success(("添加道路 " + argv[1] + " -> " + argv[2] + " (权值=" + argv[3] + ")").c_str());
    }
    else if (rc == ERR_ROAD_EXISTS)
    {
        print_error("道路已存在。");
    }
    else if (rc == ERR_CITY_NOT_FOUND)
    {
        print_error("起点或终点城市不存在。");
    }
    else if (rc == ERR_SELF_LOOP)
    {
        print_error("不允许自环道路（起点=终点）。");
    }
    else
    {
        print_error("添加道路失败。");
    }
}

void CommandParser::cmd_del_road(const std::vector<std::string> &argv,
                                 RoadNetwork &network)
{
    if (argv.size() < 3)
    {
        print_error("用法: del_road <from> <to>");
        return;
    }
    if (!is_integer(argv[1]) || !is_integer(argv[2]))
    {
        print_error("参数必须是整数。");
        return;
    }
    int from = to_int(argv[1]);
    int to = to_int(argv[2]);
    int rc = network.remove_road(from, to);
    if (rc == SUCCESS)
    {
        print_success(("删除道路 " + argv[1] + " -> " + argv[2]).c_str());
    }
    else if (rc == ERR_ROAD_NOT_FOUND)
    {
        print_error("道路不存在。");
    }
    else
    {
        print_error("删除道路失败。");
    }
}

void CommandParser::cmd_update_road(const std::vector<std::string> &argv,
                                    RoadNetwork &network)
{
    if (argv.size() < 4)
    {
        print_error("用法: update_road <from> <to> <weight>");
        return;
    }
    if (!is_integer(argv[1]) || !is_integer(argv[2]) || !is_integer(argv[3]))
    {
        print_error("参数必须是整数。");
        return;
    }
    int from = to_int(argv[1]);
    int to = to_int(argv[2]);
    int w = to_int(argv[3]);
    if (w <= 0)
    {
        print_error("道路权值必须为正整数。");
        return;
    }
    int rc = network.update_road_weight(from, to, w);
    if (rc == SUCCESS)
    {
        print_success(("更新道路 " + argv[1] + " -> " + argv[2] + " 权值=" + argv[3]).c_str());
    }
    else if (rc == ERR_ROAD_NOT_FOUND)
    {
        print_error("道路不存在。");
    }
    else
    {
        print_error("更新道路失败。");
    }
}

void CommandParser::cmd_show_network(const RoadNetwork &network)
{
    network.print_network_detail();
}

/* ============================================================ */
/*  图遍历命令                                                  */
/* ============================================================ */

void CommandParser::cmd_dfs(const std::vector<std::string> &argv,
                            RoadNetwork &network)
{
    if (argv.size() < 2)
    {
        print_error("用法: dfs <start_id>");
        return;
    }
    if (!is_integer(argv[1]))
    {
        print_error("起点ID必须是整数。");
        return;
    }
    int start = to_int(argv[1]);
    GraphBase *g = network.get_graph(STORAGE_LIST);
    int *seq = nullptr;
    int len = 0;
    int rc = traverse_dfs(g, start, &seq, &len);
    if (rc == SUCCESS && len > 0)
    {
        print_traversal_sequence(g, seq, len, "DFS");
    }
    else
    {
        print_error("DFS遍历失败（起始城市不存在或图为空）。");
    }
    if (seq)
    {
        delete[] seq;
    }
}

void CommandParser::cmd_bfs(const std::vector<std::string> &argv,
                            RoadNetwork &network)
{
    if (argv.size() < 2)
    {
        print_error("用法: bfs <start_id>");
        return;
    }
    if (!is_integer(argv[1]))
    {
        print_error("起点ID必须是整数。");
        return;
    }
    int start = to_int(argv[1]);
    GraphBase *g = network.get_graph(STORAGE_LIST);
    int *seq = nullptr;
    int len = 0;
    int rc = traverse_bfs(g, start, &seq, &len);
    if (rc == SUCCESS && len > 0)
    {
        print_traversal_sequence(g, seq, len, "BFS");
    }
    else
    {
        print_error("BFS遍历失败（起始城市不存在或图为空）。");
    }
    if (seq)
    {
        delete[] seq;
    }
}

/* ============================================================ */
/*  最短路径命令                                                */
/* ============================================================ */

void CommandParser::cmd_dijkstra(const std::vector<std::string> &argv,
                                 RoadNetwork &network)
{
    if (argv.size() < 2)
    {
        print_error("用法: dijkstra <start_id> [end_id]");
        return;
    }
    if (!is_integer(argv[1]))
    {
        print_error("起点ID必须是整数。");
        return;
    }
    int start = to_int(argv[1]);
    GraphBase *g = network.get_graph(STORAGE_LIST);
    int vcount = g->get_vertex_count();
    if (vcount == 0)
    {
        print_error("路网为空。");
        return;
    }

    int *dist = new int[vcount];
    int *prev = new int[vcount];
    int rc = run_dijkstra(g, start, dist, prev);
    if (rc != SUCCESS)
    {
        print_error("Dijkstra算法执行失败。");
        delete[] dist;
        delete[] prev;
        return;
    }

    if (argv.size() >= 3 && is_integer(argv[2]))
    {
        /* 指定终点：输出单条路径 */
        int end = to_int(argv[2]);
        int *path = nullptr;
        int plen = 0;
        if (dijkstra_get_path(prev, vcount, start, end, &path, &plen) == SUCCESS && plen > 0)
        {
            City_t c1, c2;
            g->get_vertex(start, &c1);
            g->get_vertex(end, &c2);
            std::cout << "最短路径 " << c1.name << " -> " << c2.name
                      << " : 距离=" << dist[end]
                      << ", 经过" << plen << "个城市" << std::endl;
            std::cout << "路径: ";
            for (int i = 0; i < plen; ++i)
            {
                City_t city_n;
                g->get_vertex(path[i], &city_n);
                std::cout << city_n.name;
                if (i + 1 < plen)
                    std::cout << " -> ";
            }
            std::cout << std::endl;
        }
        else
        {
            std::cout << "城市 " << end << " 不可达。" << std::endl;
        }
        if (path)
            delete[] path;
    }
    else
    {
        /* 无终点：全量输出 */
        print_dijkstra_result(g, start, dist, prev);
    }
    delete[] dist;
    delete[] prev;
}

void CommandParser::cmd_floyd(RoadNetwork &network)
{
    GraphBase *g = network.get_graph(STORAGE_LIST);
    int vcount = g->get_vertex_count();
    if (vcount == 0)
    {
        print_error("路网为空。");
        return;
    }
    int **dist = nullptr;
    int **next = nullptr;
    int rc = run_floyd(g, &dist, &next, &vcount);
    if (rc == SUCCESS)
    {
        print_floyd_table(g, dist, next, vcount);
    }
    else
    {
        print_error("Floyd算法执行失败。");
    }
    /* 释放二维数组 */
    if (dist)
    {
        for (int i = 0; i < vcount; ++i)
            delete[] dist[i];
        delete[] dist;
    }
    if (next)
    {
        for (int i = 0; i < vcount; ++i)
            delete[] next[i];
        delete[] next;
    }
}

/* ============================================================ */
/*  最小生成树命令                                              */
/* ============================================================ */

void CommandParser::cmd_prim(RoadNetwork &network)
{
    GraphBase *g = network.get_graph(STORAGE_LIST);
    if (g->get_vertex_count() == 0)
    {
        print_error("路网为空。");
        return;
    }
    MSTResult_t mst;
    std::memset(&mst, 0, sizeof(mst));
    int rc = build_mst_prim(g, &mst);
    if (rc == SUCCESS)
    {
        print_mst_result("Prim", &mst);
    }
    else if (rc == ERR_DISCONNECTED)
    {
        print_error("图不连通，无法生成最小生成树。");
    }
    else
    {
        print_error("Prim算法执行失败。");
    }
    free_mst_result(&mst);
}

void CommandParser::cmd_kruskal(RoadNetwork &network)
{
    GraphBase *g = network.get_graph(STORAGE_LIST);
    if (g->get_vertex_count() == 0)
    {
        print_error("路网为空。");
        return;
    }
    MSTResult_t mst;
    std::memset(&mst, 0, sizeof(mst));
    int rc = build_mst_kruskal(g, &mst);
    if (rc == SUCCESS)
    {
        print_mst_result("Kruskal", &mst);
    }
    else if (rc == ERR_DISCONNECTED)
    {
        print_error("图不连通，无法生成最小生成树。");
    }
    else
    {
        print_error("Kruskal算法执行失败。");
    }
    free_mst_result(&mst);
}

/* ============================================================ */
/*  拓扑排序命令                                                */
/* ============================================================ */

void CommandParser::cmd_topo(RoadNetwork &network)
{
    GraphBase *g = network.get_graph(STORAGE_LIST);
    if (g->get_vertex_count() == 0)
    {
        print_error("路网为空。");
        return;
    }
    int *seq = nullptr;
    int len = 0;
    bool cycle = false;
    int rc = run_topological_sort(g, &seq, &len, &cycle);
    if (rc == SUCCESS)
    {
        print_topo_result(g, seq, len, cycle);
    }
    else if (rc == ERR_INVALID_INPUT)
    {
        if (g->get_graph_type() != GRAPH_DIRECTED)
        {
            print_error("拓扑排序仅适用于有向图，当前路网为无向图。"
                        " 请加载有向图路网文件后重试。");
        }
        else
        {
            print_error("拓扑排序执行失败：输入参数无效。");
        }
    }
    else
    {
        print_error(("拓扑排序执行失败（错误码: " + std::to_string(rc) + "）。").c_str());
    }
    if (seq)
        delete[] seq;
}

/* ============================================================ */
/*  拥堵模拟命令                                                */
/* ============================================================ */

void CommandParser::cmd_congest(const std::vector<std::string> &argv,
                                RoadNetwork &network,
                                CongestionSimulator *simulator)
{
    if (!simulator)
    {
        print_error("拥堵模拟器未初始化。");
        return;
    }
    if (argv.size() < 4)
    {
        print_error("用法: congest <from> <to> <weight>");
        return;
    }
    if (!is_integer(argv[1]) || !is_integer(argv[2]) || !is_integer(argv[3]))
    {
        print_error("参数必须是整数。");
        return;
    }
    int from = to_int(argv[1]);
    int to = to_int(argv[2]);
    int w = to_int(argv[3]);
    int rc = simulator->set_congestion(from, to, w);
    if (rc == SUCCESS)
    {
        print_success(("设置拥堵: " + argv[1] + " -> " + argv[2] + " 权值=" + argv[3]).c_str());
    }
    else
    {
        print_error("设置拥堵失败（道路不存在或参数非法）。");
    }
}

void CommandParser::cmd_restore(RoadNetwork &network,
                                CongestionSimulator *simulator)
{
    (void)network; /* unused */
    if (!simulator)
    {
        print_error("拥堵模拟器未初始化。");
        return;
    }
    int rc = simulator->restore_all();
    if (rc == SUCCESS)
    {
        print_success("已恢复所有拥堵道路到原始权值。");
    }
    else
    {
        print_error("恢复失败。");
    }
}

/* ============================================================ */
/*  性能对比命令                                                */
/* ============================================================ */

void CommandParser::cmd_compare(RoadNetwork &network,
                                StructureComparator *comparator)
{
    (void)network;
    if (!comparator)
    {
        print_error("性能对比器未初始化。");
        return;
    }
    comparator->run_full_comparison();
}

/* ============================================================ */
/*  文件IO命令                                                  */
/* ============================================================ */

static std::string resolve_path(const std::string &input)
{
    /* 如果已含路径分隔符，直接使用；否则自动加 ./data/ 前缀 */
    if (input.find('/') != std::string::npos ||
        input.find('\\') != std::string::npos)
    {
        return input;
    }
    /* 自动补 .txt 后缀 */
    if (input.size() < 4 || input.compare(input.size() - 4, 4, ".txt") != 0)
    {
        return "./data/" + input + ".txt";
    }
    return "./data/" + input;
}

void CommandParser::cmd_save(const std::vector<std::string> &argv,
                             RoadNetwork &network)
{
    std::string fullpath;
    if (argv.size() >= 2)
    {
        fullpath = resolve_path(argv[1]);
        CommandParser::set_current_file(fullpath.c_str());
    }
    else
    {
        fullpath = CommandParser::get_current_file();
    }
    GraphBase *g = network.get_graph(STORAGE_LIST);
    int rc = FileManager::save_to_file(g, fullpath.c_str());
    if (rc == SUCCESS)
    {
        print_success(("已保存到: " + fullpath).c_str());
    }
    else if (rc == ERR_FILE_OPEN_FAIL)
    {
        print_error("文件打开失败（路径无效或无写入权限）。");
    }
    else
    {
        print_error("保存失败。");
    }
}

void CommandParser::cmd_load(const std::vector<std::string> &argv,
                             RoadNetwork &network)
{
    std::string fullpath;
    if (argv.size() >= 2)
    {
        fullpath = resolve_path(argv[1]);
    }
    else
    {
        fullpath = "./data/default.txt";
    }
    GraphBase *g = network.get_graph(STORAGE_LIST);
    int rc = FileManager::load_from_file(g, fullpath.c_str());
    if (rc == SUCCESS)
    {
        CommandParser::set_current_file(fullpath.c_str());
        /* 邻接矩阵同步 */
        GraphBase *mg = network.get_graph(STORAGE_MATRIX);
        FileManager::load_from_file(mg, fullpath.c_str());
        print_success(("已从 " + fullpath + " 加载。").c_str());
    }
    else if (rc == ERR_FILE_OPEN_FAIL)
    {
        print_error("文件打开失败（文件不存在或无权访问）。");
    }
    else if (rc == ERR_FILE_FORMAT)
    {
        print_error("文件格式错误。");
    }
    else
    {
        print_error("加载失败。");
    }
}

/* ============================================================ */
/*  文件列表 & 拥堵查询命令                                       */
/* ============================================================ */

void CommandParser::cmd_list()
{
    std::system("dir /b .\\data\\*.txt > .\\data\\_list.tmp 2>nul");
    std::ifstream infile("./data/_list.tmp");
    if (!infile.is_open())
    {
        print_error("无法列出 data/ 目录下的文件。");
        return;
    }
    std::cout << "data/ 目录下的路网文件:" << std::endl;
    std::string name;
    bool found = false;
    while (std::getline(infile, name))
    {
        while (!name.empty() && name.back() == '\r')
            name.pop_back();
        if (!name.empty())
        {
            std::cout << "  " << name << std::endl;
            found = true;
        }
    }
    if (!found)
        std::cout << "  (空)" << std::endl;
    infile.close();
    std::remove("./data/_list.tmp");
}

void CommandParser::cmd_congest_list(CongestionSimulator *simulator)
{
    if (!simulator)
    {
        print_error("拥堵模拟器未初始化。");
        return;
    }
    simulator->list_modified_roads();
}

void CommandParser::cmd_congest_report(const std::vector<std::string> &argv,
                                       CongestionSimulator *simulator)
{
    if (!simulator)
    {
        print_error("拥堵模拟器未初始化。");
        return;
    }
    if (argv.size() < 2 || !is_integer(argv[1]))
    {
        print_error("用法: congest_report <start_id>");
        return;
    }
    int start = to_int(argv[1]);
    simulator->print_comparison_report(start);
}
