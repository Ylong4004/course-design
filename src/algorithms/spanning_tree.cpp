/*
 * 模块名称  : 算法——最小生成树（Prim & Kruskal）
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : Prim 算法和 Kruskal 算法的实现，以及并查集辅助函数。
 */

#include "spanning_tree.h"
#include "union_find.h"
#include "../common/defines.h"
#include <iostream>
#include <iomanip>
#include <climits>

/* ========================== 并查集辅助函数 ========================== */

/**
 * @brief 并查集查找根节点，带路径压缩
 * @param parent 父节点数组
 * @param x 要查找的元素下标
 * @return 根节点下标
 */
int union_find_find(int* parent, int x)
{
    if (parent[x] != x) {
        parent[x] = union_find_find(parent, parent[x]); /* 路径压缩 */
    }
    return parent[x];
}

/**
 * @brief 并查集合并两个集合，按秩合并优化
 * @param parent 父节点数组
 * @param rank 秩数组
 * @param x 第一个元素下标
 * @param y 第二个元素下标
 */
void union_find_union(int* parent, int* rank, int x, int y)
{
    int root_x = union_find_find(parent, x);
    int root_y = union_find_find(parent, y);

    if (root_x == root_y) {
        return;
    }

    /* 按秩合并 */
    if (rank[root_x] < rank[root_y]) {
        parent[root_x] = root_y;
    } else if (rank[root_x] > rank[root_y]) {
        parent[root_y] = root_x;
    } else {
        parent[root_y] = root_x;
        rank[root_x]++;
    }
}

/* ========================= Prim ========================== */

/**
 * @brief Prim 算法构建最小生成树，适用于无向连通图
 * @param graph 图指针（必须为无向图）
 * @param out_mst 输出生成树结果（包含边数组、边数、总造价），调用者需调用 free_mst_result 释放
 * @return SUCCESS 或错误码
 */
int build_mst_prim(const GraphBase* graph, MSTResult_t* out_mst)
{
    if (graph == nullptr || out_mst == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int vertex_count = graph->get_vertex_count();
    if (vertex_count == 0) {
        return ERR_GRAPH_EMPTY;
    }

    if (graph->get_graph_type() == GRAPH_DIRECTED) {
        /* Prim 要求无向图，有向图直接返回错误 */
        return ERR_INVALID_INPUT;
    }

    int* all_ids = nullptr;
    int all_count = 0;
    graph->get_all_vertex_ids(&all_ids, &all_count);
    int n = all_count;

    const int max_id = graph->get_max_vertex_count();
    int* id_to_index = new int[max_id];
    for (int i = 0; i < max_id; ++i) {
        id_to_index[i] = -1;
    }
    for (int i = 0; i < n; ++i) {
        id_to_index[all_ids[i]] = i;
    }

    int* key = new int[n];
    int* parent = new int[n];
    bool* in_mst = new bool[n]();

    for (int i = 0; i < n; ++i) {
        key[i] = INF_WEIGHT;
        parent[i] = -1;
    }

    /* 从第一个顶点开始 */
    key[0] = 0;

    for (int count = 0; count < n; ++count) {
        /* 选取 key 最小的未加入顶点 */
        int min_key = INF_WEIGHT;
        int u = -1;

        for (int i = 0; i < n; ++i) {
            if (!in_mst[i] && key[i] < min_key) {
                min_key = key[i];
                u = i;
            }
        }

        if (u == -1) {
            /* 图不连通 */
            delete[] key;
            delete[] parent;
            delete[] in_mst;
            delete[] id_to_index;
            delete[] all_ids;
            return ERR_DISCONNECTED;
        }

        in_mst[u] = true;
        DEBUG_PRINT("Prim 选中顶点 " << all_ids[u] << "，key=" << min_key);

        /* 更新邻居 */
        int city_id = all_ids[u];
        Edge_t* neighbors = nullptr;
        int neighbor_count = 0;
        graph->get_neighbors(city_id, &neighbors, &neighbor_count);

        for (int i = 0; i < neighbor_count; ++i) {
            int v = id_to_index[neighbors[i].to];
            int weight = neighbors[i].weight;

            if (v >= 0 && !in_mst[v] && weight < key[v]) {
                key[v] = weight;
                parent[v] = u;
            }
        }

        delete[] neighbors;
    }

    /* 构建生成树结果 */
    /* 统计边数（根结点 0 没有 parent） */
    int edge_count = 0;
    for (int i = 1; i < n; ++i) {
        if (parent[i] != -1) {
            edge_count++;
        }
    }

    out_mst->edges = new Edge_t[edge_count];
    out_mst->edge_count = edge_count;
    out_mst->total_cost = 0;

    int idx = 0;
    for (int i = 1; i < n; ++i) {
        if (parent[i] != -1) {
            out_mst->edges[idx].from = all_ids[parent[i]];
            out_mst->edges[idx].to = all_ids[i];
            out_mst->edges[idx].weight = key[i];
            out_mst->total_cost += key[i];
            idx++;
        }
    }

    delete[] key;
    delete[] parent;
    delete[] in_mst;
    delete[] id_to_index;
    delete[] all_ids;

    return SUCCESS;
}

/* ========================= Kruskal ========================== */

/**
 * @brief Kruskal 算法构建最小生成树，使用并查集判环，适用于无向连通图
 * @param graph 图指针（必须为无向图）
 * @param out_mst 输出生成树结果（包含边数组、边数、总造价），调用者需调用 free_mst_result 释放
 * @return SUCCESS 或错误码
 */
int build_mst_kruskal(const GraphBase* graph, MSTResult_t* out_mst)
{
    if (graph == nullptr || out_mst == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int vertex_count = graph->get_vertex_count();
    if (vertex_count == 0) {
        return ERR_GRAPH_EMPTY;
    }

    if (graph->get_graph_type() == GRAPH_DIRECTED) {
        return ERR_INVALID_INPUT;
    }

    int* all_ids = nullptr;
    int all_count = 0;
    graph->get_all_vertex_ids(&all_ids, &all_count);
    int n = all_count;

    /* 收集所有边 */
    const int max_edges = n * (n - 1) / 2; /* 无向图最多边数 */
    Edge_t* all_edges = new Edge_t[max_edges];
    int edge_count = 0;

    for (int i = 0; i < all_count; ++i) {
        int city_id = all_ids[i];
        Edge_t* neighbors = nullptr;
        int neighbor_count = 0;
        graph->get_neighbors(city_id, &neighbors, &neighbor_count);

        for (int j = 0; j < neighbor_count; ++j) {
            /* 只收集 from < to 的边，避免重复 */
            if (city_id < neighbors[j].to) {
                all_edges[edge_count].from = city_id;
                all_edges[edge_count].to = neighbors[j].to;
                all_edges[edge_count].weight = neighbors[j].weight;
                edge_count++;
            }
        }

        delete[] neighbors;
    }

    if (edge_count == 0 && n > 1) {
        delete[] all_edges;
        delete[] all_ids;
        return ERR_DISCONNECTED;
    }

    /* 按权值排序（简单冒泡，边数不大） */
    for (int i = 0; i < edge_count - 1; ++i) {
        for (int j = 0; j < edge_count - 1 - i; ++j) {
            if (all_edges[j].weight > all_edges[j + 1].weight) {
                Edge_t temp = all_edges[j];
                all_edges[j] = all_edges[j + 1];
                all_edges[j + 1] = temp;
            }
        }
    }

    /* 并查集初始化 */
    int* parent = new int[n];
    int* rank = new int[n]();
    for (int i = 0; i < n; ++i) {
        parent[i] = i;
    }

    /* Kruskal 主循环 */
    const int max_id = graph->get_max_vertex_count();
    int* id_to_index = new int[max_id];
    for (int i = 0; i < max_id; ++i) {
        id_to_index[i] = -1;
    }
    for (int i = 0; i < all_count; ++i) {
        id_to_index[all_ids[i]] = i;
    }

    Edge_t* mst_edges = new Edge_t[n - 1];
    int mst_count = 0;
    int total_cost = 0;

    for (int i = 0; i < edge_count && mst_count < n - 1; ++i) {
        int idx_from = id_to_index[all_edges[i].from];
        int idx_to = id_to_index[all_edges[i].to];

        if (union_find_find(parent, idx_from) !=
            union_find_find(parent, idx_to)) {
            union_find_union(parent, rank, idx_from, idx_to);
            DEBUG_PRINT("Kruskal 选中边: " << all_edges[i].from << "→" << all_edges[i].to << " 权值=" << all_edges[i].weight);
            mst_edges[mst_count] = all_edges[i];
            mst_count++;
            total_cost += all_edges[i].weight;
        }
    }

    delete[] all_edges;
    delete[] rank;
    delete[] parent;
    delete[] id_to_index;
    delete[] all_ids;

    if (mst_count != n - 1) {
        delete[] mst_edges;
        return ERR_DISCONNECTED;
    }

    out_mst->edges = mst_edges;
    out_mst->edge_count = mst_count;
    out_mst->total_cost = total_cost;
    return SUCCESS;
}

/* ========================= 工具函数 ========================== */

/**
 * @brief 格式化打印最小生成树结果
 * @param algo_name 算法名称（如 "Prim"、"Kruskal"）
 * @param mst 生成树结果指针
 */
void print_mst_result(const char* algo_name, const MSTResult_t* mst)
{
    if (algo_name == nullptr || mst == nullptr) {
        return;
    }

    std::cout << "===== " << algo_name << " 最小生成树 =====" << std::endl;
    std::cout << "边数: " << mst->edge_count << std::endl;
    std::cout << "总造价: " << mst->total_cost << std::endl;
    std::cout << std::endl;

    std::cout << std::left << std::setw(10) << "起点"
              << std::setw(10) << "终点"
              << std::right << std::setw(10) << "权值"
              << std::endl;
    std::cout << std::string(30, '-') << std::endl;

    for (int i = 0; i < mst->edge_count; ++i) {
        std::cout << std::left << std::setw(10) << mst->edges[i].from
                  << std::setw(10) << mst->edges[i].to
                  << std::right << std::setw(10) << mst->edges[i].weight
                  << std::endl;
    }

    std::cout << std::endl;
}

/**
 * @brief 释放最小生成树结果中动态分配的边数组内存
 * @param mst 生成树结果指针
 */
void free_mst_result(MSTResult_t* mst)
{
    if (mst == nullptr) {
        return;
    }

    if (mst->edges != nullptr) {
        delete[] mst->edges;
        mst->edges = nullptr;
    }

    mst->edge_count = 0;
    mst->total_cost = 0;
}
