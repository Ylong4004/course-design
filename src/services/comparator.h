/*
 * 模块名称  : 服务——结构性能对比
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 邻接矩阵 vs 邻接表 性能横向对比统计。
 *             自动收集两套结构的内存占用、遍历耗时、查找效率等指标。
 */

#ifndef COMPARATOR_H
#define COMPARATOR_H

#include "../graph/graph_base.h"

/**
 * @class StructureComparator
 * @brief 存储结构性能对比器
 *
 * 对同一路网的邻接矩阵和邻接表分别执行同样的操作，
 * 统计各指标，输出对比报告。
 */
class StructureComparator
{
private:
    GraphBase *matrix_graph; /* 邻接矩阵图 */
    GraphBase *list_graph;   /* 邻接表图 */

public:
    StructureComparator(GraphBase *matrix_graph,
                        GraphBase *list_graph);
    ~StructureComparator();

    void set_graphs(GraphBase *new_matrix_graph,
                    GraphBase *new_list_graph);

    /* ========== 单指标统计 ========== */

    /** @brief 统计内存占用 */
    void measure_memory(size_t *out_matrix_bytes,
                       size_t *out_list_bytes) const;

    /** @brief 统计遍历耗时（所有顶点 DFS + BFS） */
    void measure_traverse_time(double *out_matrix_ms,
                             double *out_list_ms) const;

    /** @brief 统计顶点查找效率 */
    void measure_find_efficiency(int *out_matrix_cmps,
                               int *out_list_cmps) const;

    /** @brief 统计边查询效率 */
    void measure_edge_query(int *out_matrix_cmps,
                          int *out_list_cmps) const;

    /* ========== 综合报告 ========== */

    /** @brief 运行全部性能测试并输出对比报告 */
    void run_full_comparison() const;

    /** @brief 输出对比结论（推荐适用场景） */
    void print_conclusion() const;
};

#endif /* COMPARATOR_H */
