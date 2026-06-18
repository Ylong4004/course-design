/*
 * 模块名称  : 图存储——邻接矩阵实现
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 基于二维数组的图存储实现，实现 GraphBase 接口。
 *             稠密图场景表现更优，边查询 O(1)。
 */

#ifndef ADJ_MATRIX_H
#define ADJ_MATRIX_H

#include "graph_base.h"

/**
 * @class AdjMatrix
 * @brief 邻接矩阵存储的图实现
 *
 * 内部使用二维动态数组 `matrix[V][V]` 存储边权值。
 * - matrix[i][j] = INF_WEIGHT  表示 i→j 不连通
 * - matrix[i][j] = w           表示 i→j 权值为 w
 */
class AdjMatrix : public GraphBase
{
private:
    int **matrix;       /* 邻接矩阵 [rows][cols] */
    City_t *vertices;   /* 顶点数组 */
    bool *valid;        /* 顶点有效标记 */
    int vertex_count;     /* 当前顶点数 */
    int max_vertices;     /* 最大顶点容量 */
    int edge_count;       /* 当前边数 */
    GraphType graph_type; /* 图类型 */

    /* 性能统计计数器 */
    mutable int find_comparisons;
    mutable int edge_query_count;

public:
    /* ========== 构造/析构 ========== */
    AdjMatrix(int max_vertices, GraphType graph_type);
    virtual ~AdjMatrix();

    /* ========== 存储标识 ========== */
    virtual StorageType get_storage_type() const override;
    virtual const char *get_storage_name() const override;

    /* ========== 图属性查询 ========== */
    virtual int get_vertex_count() const override;
    virtual int get_edge_count() const override;
    virtual GraphType get_graph_type() const override;
    virtual int get_max_vertex_count() const override;

    /* ========== 顶点操作 ========== */
    virtual int add_vertex(const City_t &city) override;
    virtual int remove_vertex(int city_id) override;
    virtual bool has_vertex(int city_id) const override;
    virtual int get_vertex(int city_id, City_t *out_city) const override;

    /* ========== 边操作 ========== */
    virtual int add_edge(int from, int to, int weight) override;
    virtual int remove_edge(int from, int to) override;
    virtual int update_edge_weight(int from, int to, int new_weight) override;
    virtual int get_edge_weight(int from, int to, int *out_weight) const override;
    virtual bool has_edge(int from, int to) const override;

    /* ========== 遍历支持 ========== */
    virtual int get_neighbors(int vertex_id,
                             Edge_t **out_neighbors,
                             int *out_count) const override;
    virtual int get_all_vertex_ids(int **out_ids, int *out_count) const override;

    /* ========== 性能统计 ========== */
    virtual void get_performance_stats(PerfStats_t *out_stats) const override;
    virtual void reset_perf_counters() override;

    /* ========== 调试输出 ========== */
    virtual void print_graph() const override;

private:
    /** @brief 查找顶点对应的内部索引，-1 表示不存在 */
    int find_index(int city_id) const;

    /** @brief 分配并初始化矩阵内存 */
    int init_matrix();

    /** @brief 释放矩阵内存 */
    void free_matrix();
};

#endif /* ADJ_MATRIX_H */
