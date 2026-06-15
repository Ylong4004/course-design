/*
 * 模块名称  : 测试——测试用例框架
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 定义测试用例结构、测试注册与运行框架。
 *             覆盖合法输入、非法输入、边界数据三类测试。
 */

#ifndef TEST_CASES_H
#define TEST_CASES_H

#include "../graph/graph_base.h"

/* ========================== 测试用例结构 ========================== */

typedef enum
{
    TEST_LEGAL = 0,    /* 合法数据测试 */
    TEST_ILLEGAL = 1,  /* 非法数据测试 */
    TEST_BOUNDARY = 2, /* 边界数据测试 */
    TEST_PERF = 3,     /* 性能测试 */
} TestCategory;

typedef struct
{
    const char *name;      /* 测试名称 */
    TestCategory category;    /* 测试类别 */
    int (*func)();          /* 测试函数指针 */
} TestCase_t;

/**
 * @class TestRunner
 * @brief 测试运行器
 *
 * 使用方式：
 *   1. 注册测试用例 → register_test()
 *   2. 运行全部测试 → run_all()
 *   3. 按类别运行    → run_by_category()
 *   4. 查看测试报告  → print_report()
 */
class TestRunner
{
private:
    TestCase_t *cases; /* 测试用例数组 */
    int case_count;      /* 当前注册数量 */
    int max_cases;       /* 最大容量 */
    int pass_count;      /* 通过数 */
    int fail_count;      /* 失败数 */

public:
    TestRunner(int max_cases);
    ~TestRunner();

    /** @brief 注册一个测试用例 */
    void register_test(const char *name,
                      TestCategory cat,
                      int (*func)());

    /** @brief 运行全部已注册的测试 */
    void run_all();

    /** @brief 按类别运行测试 */
    void run_by_category(TestCategory cat);

    /** @brief 打印测试报告 */
    void print_report() const;

private:
    void run_single(const TestCase_t *test_case);
};

/* ========================== 预定义测试用例 ========================== */

/**
 * 以下为推荐的测试用例清单，由组员 C 负责实现具体测试逻辑
 */

/* --- 路网构建测试 --- */
int test_add_city_legal();          /* 合法：正常添加城市 */
int test_add_city_duplicate();      /* 非法：添加重复城市 */
int test_add_city_boundary_max();    /* 边界：添加到容量上限 */
int test_remove_city_not_found();    /* 非法：删除不存在的城市 */
int test_add_road_legal();          /* 合法：正常添加道路 */
int test_add_road_duplicate();      /* 非法：添加重复道路 */
int test_add_road_self_loop();       /* 非法：自环边 */
int test_add_road_weight_negative(); /* 非法：负权值（Dijkstra 不适用） */

/* --- 遍历测试 --- */
int test_dfs_disconnected(); /* 边界：不连通图的 DFS */
int test_bfs_single_vertex(); /* 边界：单顶点图 */
int test_traversal_empty();  /* 非法：空图遍历 */

/* --- 最短路径测试 --- */
int test_dijkstra_normal();       /* 合法：正常最短路径 */
int test_dijkstra_no_path();       /* 边界：无路径连通 */
int test_floyd_compare_dijkstra(); /* 验证：Floyd 结果与 Dijkstra 一致 */

/* --- 最小生成树测试 --- */
int test_mst_normal();        /* 合法：正常生成树 */
int test_mst_disconnected();  /* 非法：不连通图 */
int test_mst_prim_vs_kruskal(); /* 验证：两种算法结果一致 */

/* --- 拓扑排序测试 --- */
int test_topo_dag();             /* 合法：有向无环图 */
int test_topo_has_cycle();        /* 非法：有环图 */
int test_topo_undirected_error(); /* 非法：无向图调用拓扑排序 */

/* --- 文件 IO 测试 --- */
int test_save_load_round_trip(); /* 合法：保存再加载，数据一致 */
int test_load_file_not_found();  /* 非法：文件不存在 */
int test_load_bad_format();     /* 非法：文件格式错误 */

#endif /* TEST_CASES_H */
