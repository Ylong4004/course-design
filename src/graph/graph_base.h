/*
 * 模块名称  : 图存储——抽象基类
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 定义图存储的统一抽象接口，邻接矩阵与邻接表均实现此接口。
 *             上层算法模块仅依赖此接口，实现"算法一份代码、两种存储复用"。
 */

#ifndef GRAPH_BASE_H
#define GRAPH_BASE_H

#include "../common/types.h"

/**
 * @class GraphBase
 * @brief 图存储结构抽象基类
 *
 * 所有图存储结构（邻接矩阵、邻接表）必须实现以下纯虚接口。
 * 上层算法通过 GraphBase* 指针操作，无需感知底层存储细节。
 */
class GraphBase
{
public:
    /* ==================== 构造/析构 ==================== */

    GraphBase() {}
    virtual ~GraphBase() {}

    /* ==================== 存储标识 ==================== */

    /** @return 当前存储类型：STORAGE_MATRIX 或 STORAGE_LIST */
    virtual StorageType get_storage_type() const = 0;

    /** @return 存储结构的可读名称，如 "邻接矩阵" */
    virtual const char *get_storage_name() const = 0;

    /* ==================== 图属性查询 ==================== */

    /** @return 当前城市（顶点）数量 */
    virtual int get_vertex_count() const = 0;

    /** @return 当前道路（边）数量 */
    virtual int get_edge_count() const = 0;

    /** @return 图类型：无向图或有向图 */
    virtual GraphType get_graph_type() const = 0;

    /** @return 最大城市容量 */
    virtual int get_max_vertex_count() const = 0;

    /* ==================== 顶点操作 ==================== */

    /**
     * @brief  添加一个城市顶点
     * @param  city  城市信息（含编号、名称）
     * @return 成功返回 SUCCESS，失败返回错误码
     */
    virtual int add_vertex(const City_t &city) = 0;

    /**
     * @brief  删除指定城市及关联的所有道路
     * @param  city_id  城市编号
     * @return 成功返回 SUCCESS，失败返回错误码
     */
    virtual int remove_vertex(int city_id) = 0;

    /**
     * @brief  查询城市是否存在
     * @param  city_id  城市编号
     * @return 存在返回 true，否则返回 false
     */
    virtual bool has_vertex(int city_id) const = 0;

    /**
     * @brief  根据编号获取城市信息
     * @param  city_id  城市编号
     * @param  out_city 输出：城市信息
     * @return 成功返回 SUCCESS
     */
    virtual int get_vertex(int city_id, City_t *out_city) const = 0;

    /* ==================== 边操作 ==================== */

    /**
     * @brief  添加一条道路（边）
     * @param  from   起点城市编号
     * @param  to     终点城市编号
     * @param  weight 边权值（距离/耗时）
     * @return 成功返回 SUCCESS，失败返回错误码
     */
    virtual int add_edge(int from, int to, int weight) = 0;

    /**
     * @brief  删除一条道路
     * @param  from  起点城市编号
     * @param  to    终点城市编号
     * @return 成功返回 SUCCESS
     */
    virtual int remove_edge(int from, int to) = 0;

    /**
     * @brief  修改道路权值
     * @param  from      起点城市编号
     * @param  to        终点城市编号
     * @param  new_weight 新权值
     * @return 成功返回 SUCCESS
     */
    virtual int update_edge_weight(int from, int to, int new_weight) = 0;

    /**
     * @brief  查询边权值
     * @param  from      起点城市编号
     * @param  to        终点城市编号
     * @param  out_weight 输出：边权值
     * @return 成功返回 SUCCESS
     */
    virtual int get_edge_weight(int from, int to, int *out_weight) const = 0;

    /**
     * @brief  查询两城市是否直接相连
     * @return 相连返回 true
     */
    virtual bool has_edge(int from, int to) const = 0;

    /* ==================== 遍历支持 ==================== */

    /**
     * @brief  获取指定顶点的所有邻接顶点
     * @param  vertex_id  顶点编号
     * @param  out_neighbors  输出：邻接顶点数组（含权值）
     * @param  out_count      输出：邻接顶点个数
     * @return 成功返回 SUCCESS
     * @note   调用方负责释放 out_neighbors 内存
     */
    virtual int get_neighbors(int vertex_id,
                             Edge_t **out_neighbors,
                             int *out_count) const = 0;

    /**
     * @brief  获取所有顶点的编号列表
     * @param  out_ids    输出：顶点编号数组
     * @param  out_count  输出：顶点数量
     * @note   调用方负责释放 out_ids 内存
     */
    virtual int get_all_vertex_ids(int **out_ids, int *out_count) const = 0;

    /* ==================== 性能统计 ==================== */

    /**
     * @brief  获取当前存储结构的性能指标
     * @param  out_stats  输出：性能统计数据
     */
    virtual void get_performance_stats(PerfStats_t *out_stats) const = 0;

    /**
     * @brief  重置性能计数器（用于新一轮统计）
     */
    virtual void reset_perf_counters() = 0;

    /* ==================== 调试输出 ==================== */

    /** @brief 打印当前图结构（用于调试/展示） */
    virtual void print_graph() const = 0;
};

#endif /* GRAPH_BASE_H */
