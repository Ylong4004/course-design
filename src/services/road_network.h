/*
 * 模块名称  : 服务——路网管理
 * 编写人    : 组员A（架构负责人） + 组员B（核心功能）
 * 功能描述  : 路网构建、编辑、查询等业务操作。
 *             同时管理邻接矩阵和邻接表两套存储，保证数据同步。
 */

#ifndef ROAD_NETWORK_H
#define ROAD_NETWORK_H

#include "../graph/graph_base.h"
#include "../graph/adj_matrix.h"
#include "../graph/adj_list.h"

/**
 * @class RoadNetwork
 * @brief 路网管理器——同时维护邻接矩阵和邻接表两套图结构
 *
 * 所有路网编辑操作在两套结构上同步执行。
 * 提供统一的城市/道路增删改接口。
 */
class RoadNetwork
{
private:
    GraphBase *matrix_graph; /* 邻接矩阵存储 */
    GraphBase *list_graph;   /* 邻接表存储 */
    GraphType graph_type;     /* 图类型 */
    int max_cities;           /* 最大城市数 */

public:
    /**
     * @brief 构造路网，同时初始化两套存储
     * @param max_vertices  最大城市容量
     * @param graph_type      图类型（有向/无向）
     */
    RoadNetwork(int max_vertices, GraphType graph_type);
    ~RoadNetwork();

    /** @brief 获取指定存储类型的图对象指针 */
    GraphBase *get_graph(StorageType type);

    /** @brief 获取图类型 */
    GraphType get_type() const;

    /* ========== 城市管理（双结构同步） ========== */

    int add_city(int id, const char *name);
    int remove_city(int city_id);
    bool has_city(int city_id) const;
    int get_city_name(int city_id, char *out_name) const;
    int get_city_count() const;
    void list_all_cities() const;

    /* ========== 道路管理（双结构同步） ========== */

    int add_road(int from, int to, int weight);
    int remove_road(int from, int to);
    int update_road_weight(int from, int to, int new_weight);
    int get_road_weight(int from, int to) const;
    bool has_road(int from, int to) const;
    int get_road_count() const;

    /* ========== 路网查询 ========== */

    /** @brief 打印路网总览 */
    void print_network_overview() const;

    /** @brief 分别打印两种存储结构 */
    void print_both_structures() const;
};

#endif /* ROAD_NETWORK_H */
