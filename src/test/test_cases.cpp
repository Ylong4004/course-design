/*
 * 模块名称  : 测试——测试用例实现
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : TestRunner 类实现 + 23 个测试用例。
 */

#include "../common/defines.h"
#include "test_cases.h"
#include "../graph/adj_matrix.h"
#include "../graph/adj_list.h"
#include "../algorithms/traversal.h"
#include "../algorithms/shortest_path.h"
#include "../algorithms/spanning_tree.h"
#include "../algorithms/topological.h"
#include "../services/file_io.h"
#include <iostream>
#include <cstring>

/* ======================== 辅助工具 ======================== */

/** @brief 创建一个含 5 个城市的无向图（三角形+分支） */
static GraphBase *create_sample_undirected_graph()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "北京");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "上海");
    g->add_vertex(c);
    c.id = 3;
    std::strcpy(c.name, "广州");
    g->add_vertex(c);
    c.id = 4;
    std::strcpy(c.name, "深圳");
    g->add_vertex(c);
    c.id = 5;
    std::strcpy(c.name, "杭州");
    g->add_vertex(c);
    g->add_edge(1, 2, 1200);
    g->add_edge(1, 3, 2100);
    g->add_edge(2, 3, 1400);
    g->add_edge(2, 5, 180);
    g->add_edge(3, 4, 150);
    return g;
}

/** @brief 创建一个有向无环图（DAG） */
static GraphBase *create_sample_dag()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_DIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "A");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "B");
    g->add_vertex(c);
    c.id = 3;
    std::strcpy(c.name, "C");
    g->add_vertex(c);
    c.id = 4;
    std::strcpy(c.name, "D");
    g->add_vertex(c);
    g->add_edge(1, 2, 1);
    g->add_edge(1, 3, 1);
    g->add_edge(2, 4, 1);
    g->add_edge(3, 4, 1);
    return g;
}

/** @brief 创建含环的有向图 */
static GraphBase *create_cyclic_digraph()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_DIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "X");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "Y");
    g->add_vertex(c);
    c.id = 3;
    std::strcpy(c.name, "Z");
    g->add_vertex(c);
    g->add_edge(1, 2, 1);
    g->add_edge(2, 3, 1);
    g->add_edge(3, 1, 1); /* 环路 */
    return g;
}

/* ======================== TestRunner 实现 ======================== */

/**
 * @brief 构造函数，初始化测试运行器，分配测试用例数组
 * @param max_cases 最大测试用例数
 */
TestRunner::TestRunner(int max_cases)
{
    this->max_cases = max_cases > 0 ? max_cases : 50;
    safe_new_array(cases, TestCase_t, this->max_cases);
    case_count = 0;
    pass_count = 0;
    fail_count = 0;
}

/**
 * @brief 析构函数，释放测试用例数组内存
 */
TestRunner::~TestRunner()
{
    safe_delete_array(cases);
}

/**
 * @brief 注册一个测试用例到运行器中
 * @param name 测试名称
 * @param cat 测试分类（合法/非法/边界/性能）
 * @param func 测试函数指针
 */
void TestRunner::register_test(const char *name,
                               TestCategory cat,
                               int (*func)())
{
    if (case_count >= max_cases || name == nullptr || func == nullptr)
    {
        return;
    }
    cases[case_count].name = name;
    cases[case_count].category = cat;
    cases[case_count].func = func;
    ++case_count;
}

/**
 * @brief 运行全部已注册的测试用例并输出汇总报告
 */
void TestRunner::run_all()
{
    pass_count = 0;
    fail_count = 0;

    std::cout << "========== 全量测试 ==========" << std::endl;
    for (int i = 0; i < case_count; ++i)
    {
        run_single(&cases[i]);
    }
    print_report();
}

/**
 * @brief 按分类运行测试用例（如仅运行非法数据测试）
 * @param cat 测试分类
 */
void TestRunner::run_by_category(TestCategory cat)
{
    pass_count = 0;
    fail_count = 0;

    const char *cat_name[] = {"合法", "非法", "边界", "性能"};
    std::cout << "========== " << cat_name[cat]
              << "数据测试 ==========" << std::endl;

    for (int i = 0; i < case_count; ++i)
    {
        if (cases[i].category == cat)
        {
            run_single(&cases[i]);
        }
    }
    print_report();
}

/**
 * @brief 运行单个测试用例，根据返回值判定通过/失败
 * @param test_case 测试用例指针
 */
void TestRunner::run_single(const TestCase_t *test_case)
{
    std::cout << "  测试: " << test_case->name << " ... ";
    int result = test_case->func();
    if (result == SUCCESS)
    {
        ++pass_count;
        std::cout << "✓ 通过" << std::endl;
    }
    else
    {
        ++fail_count;
        std::cout << "✗ 失败 (错误码: " << result << ")" << std::endl;
    }
}

/**
 * @brief 打印测试汇总报告（总计/通过/失败数量）
 */
void TestRunner::print_report() const
{
    int total = pass_count + fail_count;
    std::cout << std::endl;
    std::cout << "---------- 测试报告 ----------" << std::endl;
    std::cout << "  总计: " << total
              << "  |  通过: " << pass_count
              << "  |  失败: " << fail_count << std::endl;
    std::cout << "==============================" << std::endl;
}

/* ======================== 路网构建测试 ======================== */

/**
 * @brief 合法数据：添加单个城市，验证返回 SUCCESS
 * @return SUCCESS 或错误码
 */
int test_add_city_legal()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "北京");
    int result = g->add_vertex(c);
    delete g;
    return result;
}

/**
 * @brief 非法数据：添加重复编号城市，验证返回 ERR_CITY_DUPLICATE
 * @return SUCCESS 或错误码
 */
int test_add_city_duplicate()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "北京");
    g->add_vertex(c);
    c.id = 1;
    std::strcpy(c.name, "南京");
    int result = g->add_vertex(c);
    delete g;
    return (result == ERR_CITY_DUPLICATE) ? SUCCESS : ERR_CITY_DUPLICATE;
}

/**
 * @brief 边界数据：添加城市至容量上限后继续添加，验证返回 ERR_GRAPH_FULL
 * @return SUCCESS 或错误码
 */
int test_add_city_boundary_max()
{
    const int max = 3;
    GraphBase *g = new AdjMatrix(max, GRAPH_UNDIRECTED);
    City_t c;
    for (int i = 0; i < max; ++i)
    {
        c.id = i + 1;
        c.name[0] = 'A' + i;
        c.name[1] = '\0';
        g->add_vertex(c);
    }
    c.id = 999;
    std::strcpy(c.name, "溢出");
    int result = g->add_vertex(c);
    delete g;
    return (result == ERR_GRAPH_FULL) ? SUCCESS : ERR_GRAPH_FULL;
}

/**
 * @brief 非法数据：删除不存在的城市，验证返回 ERR_CITY_NOT_FOUND
 * @return SUCCESS 或错误码
 */
int test_remove_city_not_found()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    int result = g->remove_vertex(999);
    delete g;
    return (result == ERR_CITY_NOT_FOUND) ? SUCCESS : ERR_CITY_NOT_FOUND;
}

/**
 * @brief 合法数据：在两个已存在城市间添加道路，验证返回 SUCCESS
 * @return SUCCESS 或错误码
 */
int test_add_road_legal()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "A");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "B");
    g->add_vertex(c);
    int result = g->add_edge(1, 2, 100);
    delete g;
    return result;
}

/**
 * @brief 非法数据：重复添加已存在道路，验证返回 ERR_ROAD_EXISTS
 * @return SUCCESS 或错误码
 */
int test_add_road_duplicate()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "A");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "B");
    g->add_vertex(c);
    g->add_edge(1, 2, 100);
    int result = g->add_edge(1, 2, 200);
    delete g;
    return (result == ERR_ROAD_EXISTS) ? SUCCESS : ERR_ROAD_EXISTS;
}

/**
 * @brief 非法数据：添加自环道路（起点=终点），验证返回 ERR_SELF_LOOP
 * @return SUCCESS 或错误码
 */
int test_add_road_self_loop()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "A");
    g->add_vertex(c);
    int result = g->add_edge(1, 1, 100);
    delete g;
    return (result == ERR_SELF_LOOP) ? SUCCESS : ERR_SELF_LOOP;
}

/**
 * @brief 非法数据：添加负权值道路，验证返回 ERR_INVALID_INPUT
 * @return SUCCESS 或错误码
 */
int test_add_road_weight_negative()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "A");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "B");
    g->add_vertex(c);
    int result = g->add_edge(1, 2, -100);
    delete g;
    return (result == ERR_INVALID_INPUT) ? SUCCESS : ERR_INVALID_INPUT;
}

/* ======================== 遍历测试 ======================== */

/**
 * @brief 边界数据：DFS 遍历非连通图（两个孤立顶点），验证能输出全部顶点
 * @return SUCCESS 或错误码
 */
int test_dfs_disconnected()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "孤岛1");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "孤岛2");
    g->add_vertex(c);
    int *seq = nullptr;
    int len = 0;
    int result = traverse_dfs(g, 1, &seq, &len);
    bool ok = (result == SUCCESS && len == 2);
    delete[] seq;
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/**
 * @brief 边界数据：BFS 遍历单顶点图，验证序列长度=1
 * @return SUCCESS 或错误码
 */
int test_bfs_single_vertex()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "单城");
    g->add_vertex(c);
    int *seq = nullptr;
    int len = 0;
    int result = traverse_bfs(g, 1, &seq, &len);
    bool ok = (result == SUCCESS && len == 1);
    delete[] seq;
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/**
 * @brief 非法数据：遍历空图，验证返回 ERR_GRAPH_EMPTY
 * @return SUCCESS 或错误码
 */
int test_traversal_empty()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    int *seq = nullptr;
    int len = 0;
    int result = traverse_dfs(g, 1, &seq, &len);
    delete g;
    return (result == ERR_GRAPH_EMPTY) ? SUCCESS : ERR_GRAPH_EMPTY;
}

/* ======================== 最短路径测试 ======================== */

/**
 * @brief 合法数据：Dijkstra 正常最短路径，验证北京→深圳距离=2250
 * @return SUCCESS 或错误码
 */
int test_dijkstra_normal()
{
    GraphBase *g = create_sample_undirected_graph();
    int n = g->get_vertex_count();
    int *dist = new int[n];
    int *prev = new int[n];
    int result = run_dijkstra(g, 1, dist, prev);
    /* 北京→深圳 = 北京→广州→深圳 = 2100+150 = 2250 */
    bool ok = (result == SUCCESS && dist[3] == 2250);
    delete[] dist;
    delete[] prev;
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/**
 * @brief 边界数据：Dijkstra 不连通顶点间最短路径，验证距离=INF_WEIGHT
 * @return SUCCESS 或错误码
 */
int test_dijkstra_no_path()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "孤城A");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "孤城B");
    g->add_vertex(c);
    int *dist = new int[2];
    int *prev = new int[2];
    int result = run_dijkstra(g, 1, dist, prev);
    bool ok = (result == SUCCESS && dist[1] == INF_WEIGHT);
    delete[] dist;
    delete[] prev;
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/**
 * @brief 性能/交叉验证：Floyd 与 Dijkstra 结果一致性对比
 * @return SUCCESS 或错误码
 */
int test_floyd_compare_dijkstra()
{
    GraphBase *g = create_sample_undirected_graph();
    int ***floyd_dist = new int **;
    int ***floyd_next = new int **;
    int vc = 0;
    run_floyd(g, floyd_dist, floyd_next, &vc);

    int *dij_dist = new int[vc];
    int *dij_prev = new int[vc];
    run_dijkstra(g, 1, dij_dist, dij_prev);

    bool ok = ((*floyd_dist)[0][1] == dij_dist[1]);

    for (int i = 0; i < vc; ++i)
    {
        delete[] (*floyd_dist)[i];
        delete[] (*floyd_next)[i];
    }
    delete[] *floyd_dist;
    delete[] *floyd_next;
    delete floyd_dist;
    delete floyd_next;
    delete[] dij_dist;
    delete[] dij_prev;
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/* ======================== 最小生成树测试 ======================== */

/**
 * @brief 合法数据：Prim 最小生成树正常情况，验证边数=顶点数-1
 * @return SUCCESS 或错误码
 */
int test_mst_normal()
{
    GraphBase *g = create_sample_undirected_graph();
    MSTResult_t mst;
    int result = build_mst_prim(g, &mst);
    int vc = g->get_vertex_count();
    bool ok = (result == SUCCESS && mst.edge_count == vc - 1);
    free_mst_result(&mst);
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/**
 * @brief 非法数据：MST 不连通图，验证返回 ERR_DISCONNECTED
 * @return SUCCESS 或错误码
 */
int test_mst_disconnected()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "孤城A");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "孤城B");
    g->add_vertex(c);
    MSTResult_t mst;
    int result = build_mst_prim(g, &mst);
    delete g;
    return (result == ERR_DISCONNECTED) ? SUCCESS : ERR_DISCONNECTED;
}

/**
 * @brief 性能/交叉验证：Prim 与 Kruskal 总造价一致性对比
 * @return SUCCESS 或错误码
 */
int test_mst_prim_vs_kruskal()
{
    GraphBase *g = create_sample_undirected_graph();
    MSTResult_t mst1, mst2;
    build_mst_prim(g, &mst1);
    build_mst_kruskal(g, &mst2);
    bool ok = (mst1.total_cost == mst2.total_cost);
    free_mst_result(&mst1);
    free_mst_result(&mst2);
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/* ======================== 拓扑排序测试 ======================== */

/**
 * @brief 合法数据：DAG 拓扑排序，验证无环且序列完整
 * @return SUCCESS 或错误码
 */
int test_topo_dag()
{
    GraphBase *g = create_sample_dag();
    int *seq = nullptr;
    int len = 0;
    bool has_cycle = true;
    int result = run_topological_sort(g, &seq, &len, &has_cycle);
    /* DAG 应有完整序列且无环 */
    bool ok = (result == SUCCESS && !has_cycle && len == 4);
    delete[] seq;
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/**
 * @brief 非法数据：含环有向图拓扑排序，验证检测到环路
 * @return SUCCESS 或错误码
 */
int test_topo_has_cycle()
{
    GraphBase *g = create_cyclic_digraph();
    int *seq = nullptr;
    int len = 0;
    bool has_cycle = false;
    run_topological_sort(g, &seq, &len, &has_cycle);
    bool ok = has_cycle;
    delete[] seq;
    delete g;
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/**
 * @brief 非法数据：对无向图运行拓扑排序，验证返回 ERR_INVALID_INPUT
 * @return SUCCESS 或错误码
 */
int test_topo_undirected_error()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    City_t c;
    c.id = 1;
    std::strcpy(c.name, "A");
    g->add_vertex(c);
    c.id = 2;
    std::strcpy(c.name, "B");
    g->add_vertex(c);
    g->add_edge(1, 2, 1);
    int *seq = nullptr;
    int len = 0;
    bool has_cycle = false;
    int result = run_topological_sort(g, &seq, &len, &has_cycle);
    delete[] seq;
    delete g;
    return (result == ERR_INVALID_INPUT) ? SUCCESS : ERR_INVALID_INPUT;
}

/* ======================== 文件 IO 测试 ======================== */

/**
 * @brief 合法数据：保存路网到文件后重新加载，验证顶点数和边数一致
 * @return SUCCESS 或错误码
 */
int test_save_load_round_trip()
{
    GraphBase *g = create_sample_undirected_graph();
    const char *test_path = "./data/test_roundtrip.txt";

    int save_ok = FileManager::save_to_file(g, test_path);
    if (save_ok != SUCCESS)
    {
        std::cerr << "[测试] 保存失败: " << test_path
                  << " (错误码 " << save_ok << ")" << std::endl;
        delete g;
        return save_ok;
    }

    GraphBase *loaded = new AdjMatrix(10, GRAPH_UNDIRECTED);
    int load_ok = FileManager::load_from_file(loaded, test_path);
    if (load_ok != SUCCESS)
    {
        delete g;
        delete loaded;
        return load_ok;
    }

    bool ok = (g->get_vertex_count() == loaded->get_vertex_count() &&
               g->get_edge_count() == loaded->get_edge_count());

    delete g;
    delete loaded;
    /* 清理测试文件 */
    std::remove(test_path);
    return ok ? SUCCESS : ERR_INVALID_INPUT;
}

/**
 * @brief 非法数据：加载不存在的文件，验证返回 ERR_FILE_OPEN_FAIL
 * @return SUCCESS 或错误码
 */
int test_load_file_not_found()
{
    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    int result = FileManager::load_from_file(g, "./data/__nonexistent__.txt");
    delete g;
    return (result == ERR_FILE_OPEN_FAIL) ? SUCCESS : ERR_FILE_OPEN_FAIL;
}

/**
 * @brief 非法数据：加载格式错误的文件，验证返回 ERR_FILE_FORMAT
 * @return SUCCESS 或错误码
 */
int test_load_bad_format()
{
    /* 创建一个格式错误的文件 */
    const char *bad_path = "./data/test_bad.txt";
    std::ofstream out(bad_path);
    out << "GARBAGE DATA 123\nNOT VALID FORMAT\n";
    out.close();

    GraphBase *g = new AdjMatrix(10, GRAPH_UNDIRECTED);
    int result = FileManager::load_from_file(g, bad_path);
    delete g;
    std::remove(bad_path);
    return (result == ERR_FILE_FORMAT) ? SUCCESS : ERR_FILE_FORMAT;
}

/* ======================== 运行入口 ======================== */

/**
 * @brief  独立运行全部测试
 * @note   在 main 中调用或在菜单中触发
 */
int run_all_tests()
{
    TestRunner runner(50);

    /* 路网构建 */
    runner.register_test("添加城市(合法)", TEST_LEGAL, test_add_city_legal);
    runner.register_test("添加城市(重复)", TEST_ILLEGAL, test_add_city_duplicate);
    runner.register_test("添加城市(边界-容量满)", TEST_BOUNDARY, test_add_city_boundary_max);
    runner.register_test("删除城市(不存在)", TEST_ILLEGAL, test_remove_city_not_found);
    runner.register_test("添加道路(合法)", TEST_LEGAL, test_add_road_legal);
    runner.register_test("添加道路(重复)", TEST_ILLEGAL, test_add_road_duplicate);
    runner.register_test("添加道路(自环)", TEST_ILLEGAL, test_add_road_self_loop);
    runner.register_test("添加道路(负权值)", TEST_ILLEGAL, test_add_road_weight_negative);

    /* 遍历 */
    runner.register_test("DFS 非连通图", TEST_BOUNDARY, test_dfs_disconnected);
    runner.register_test("BFS 单顶点", TEST_BOUNDARY, test_bfs_single_vertex);
    runner.register_test("遍历空图", TEST_ILLEGAL, test_traversal_empty);

    /* 最短路径 */
    runner.register_test("Dijkstra 正常", TEST_LEGAL, test_dijkstra_normal);
    runner.register_test("Dijkstra 无路径", TEST_BOUNDARY, test_dijkstra_no_path);
    runner.register_test("Floyd vs Dijkstra 交叉验证", TEST_PERF, test_floyd_compare_dijkstra);

    /* 最小生成树 */
    runner.register_test("MST 正常", TEST_LEGAL, test_mst_normal);
    runner.register_test("MST 不连通图", TEST_ILLEGAL, test_mst_disconnected);
    runner.register_test("Prim vs Kruskal 交叉验证", TEST_PERF, test_mst_prim_vs_kruskal);

    /* 拓扑排序 */
    runner.register_test("DAG 拓扑排序", TEST_LEGAL, test_topo_dag);
    runner.register_test("有环图", TEST_ILLEGAL, test_topo_has_cycle);
    runner.register_test("无向图错误", TEST_ILLEGAL, test_topo_undirected_error);

    /* 文件 IO */
    runner.register_test("保存加载回环", TEST_LEGAL, test_save_load_round_trip);
    runner.register_test("文件不存在", TEST_ILLEGAL, test_load_file_not_found);
    runner.register_test("格式错误", TEST_ILLEGAL, test_load_bad_format);

    runner.run_all();
    return 0;
}
