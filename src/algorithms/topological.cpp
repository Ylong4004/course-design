/*
 * 模块名称  : 算法——拓扑排序
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 拓扑排序（Kahn 算法）和环路检测的实现。
 */

#include "topological.h"
#include "queue.h"
#include "../common/defines.h"
#include <iostream>
#include <iomanip>

/* ========================= 拓扑排序（Kahn 算法） ========================== */

/**
 * @brief 拓扑排序（Kahn 算法），使用队列依次输出入度为 0 的顶点
 * @param graph 图指针（必须为有向图）
 * @param out_sequence 输出拓扑序列（调用者需 delete[]）
 * @param out_length 输出序列长度
 * @param has_cycle 输出是否存在环路
 * @return SUCCESS 或错误码
 */
int run_topological_sort(const GraphBase* graph,
                         int** out_sequence,
                         int* out_length,
                         bool* has_cycle)
{
    if (graph == nullptr || out_sequence == nullptr ||
        out_length == nullptr || has_cycle == nullptr) {
        return ERR_INVALID_INPUT;
    }

    /* 拓扑排序仅适用于有向图 */
    if (graph->get_graph_type() != GRAPH_DIRECTED) {
        return ERR_INVALID_INPUT;
    }

    int vertex_count = graph->get_vertex_count();
    if (vertex_count == 0) {
        return ERR_GRAPH_EMPTY;
    }

    int* all_ids = nullptr;
    int all_count = 0;
    graph->get_all_vertex_ids(&all_ids, &all_count);

    const int max_id = graph->get_max_vertex_count();

    /* 建立 city_id → 内部下标 映射 */
    int* id_to_index = new int[max_id];
    for (int i = 0; i < max_id; ++i) {
        id_to_index[i] = -1;
    }
    for (int i = 0; i < all_count; ++i) {
        id_to_index[all_ids[i]] = i;
    }

    /* 计算入度 */
    int n = all_count;
    int* indegree = new int[n]();

    for (int i = 0; i < all_count; ++i) {
        Edge_t* neighbors = nullptr;
        int neighbor_count = 0;
        graph->get_neighbors(all_ids[i], &neighbors, &neighbor_count);

        for (int j = 0; j < neighbor_count; ++j) {
            int v = id_to_index[neighbors[j].to];
            if (v >= 0) {
                indegree[v]++;
            }
        }

        delete[] neighbors;
    }

    /* 入度为 0 的顶点入队 */
    Queue queue(n);
    for (int i = 0; i < n; ++i) {
        if (indegree[i] == 0) {
            queue.push(i); /* 存内部下标 */
        }
    }

    /* 分配结果数组 */
    int* sequence = new int[n];
    int seq_len = 0;

    while (!queue.empty()) {
        int u = queue.pop();
        DEBUG_PRINT("拓扑排序 出队顶点 " << all_ids[u] << "，当前入度=0");
        sequence[seq_len++] = all_ids[u];

        /* 删除 u 的所有出边（邻居入度 -1） */
        Edge_t* neighbors = nullptr;
        int neighbor_count = 0;
        graph->get_neighbors(all_ids[u], &neighbors, &neighbor_count);

        for (int i = 0; i < neighbor_count; ++i) {
            int v = id_to_index[neighbors[i].to];
            if (v >= 0) {
                indegree[v]--;
                if (indegree[v] == 0) {
                    DEBUG_PRINT("  顶点 " << all_ids[v] << " 入度降为 0，入队");
                    queue.push(v);
                }
            }
        }

        delete[] neighbors;
    }

    /* 判断是否有环路 */
    if (seq_len < n) {
        *has_cycle = true;
    } else {
        *has_cycle = false;
    }

    delete[] indegree;
    delete[] id_to_index;
    delete[] all_ids;

    *out_sequence = sequence;
    *out_length = seq_len;
    return SUCCESS;
}

/* ========================= 环路检测快捷接口 ========================== */

/**
 * @brief 环路检测快捷接口，内部调用拓扑排序判断图中是否存在环
 * @param graph 图指针
 * @return 存在环返回 true，否则返回 false
 */
bool has_cycle(const GraphBase* graph)
{
    if (graph == nullptr) {
        return false;
    }

    int* sequence = nullptr;
    int length = 0;
    bool cycle = false;

    run_topological_sort(graph, &sequence, &length, &cycle);

    if (sequence != nullptr) {
        delete[] sequence;
    }

    return cycle;
}

/* ========================= 格式化输出 ========================== */

/**
 * @brief 格式化打印拓扑排序结果，有环时输出死锁分析提示
 * @param graph 图指针
 * @param sequence 拓扑序列数组
 * @param length 序列长度
 * @param has_cycle 是否存在环路
 */
void print_topo_result(const GraphBase* graph,
                       const int* sequence,
                       int length,
                       bool has_cycle)
{
    if (graph == nullptr || sequence == nullptr) {
        return;
    }

    std::cout << "===== 拓扑排序结果 =====" << std::endl;

    if (has_cycle) {
        std::cout << "⚠ 图中存在环路，无法完成完整的拓扑排序！" << std::endl;
        std::cout << "仅输出 " << length << " 个顶点的合法序列："
                  << std::endl;
    } else {
        std::cout << "✓ 图为有向无环图（DAG），拓扑排序完成。" << std::endl;
        std::cout << "共 " << length << " 个顶点：" << std::endl;
    }

    std::cout << std::endl;

    for (int i = 0; i < length; ++i) {
        City_t city;
        if (graph->get_vertex(sequence[i], &city) == SUCCESS) {
            std::cout << "  [" << i + 1 << "] " << city.name
                      << " (编号: " << city.id << ")" << std::endl;
        }
    }

    if (has_cycle) {
        std::cout << std::endl;
        std::cout << "【交通死锁分析】环形通路存在，可能引发交通死锁问题！"
                  << std::endl;
        std::cout << "建议修改道路方向或增加单向道以打破环路。" << std::endl;
    }

    std::cout << std::endl;
}
