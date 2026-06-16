/*
 * 模块名称  : 算法——图遍历（DFS & BFS）
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 深度优先遍历和广度优先遍历的实现。
 */

#include "traversal.h"
#include "stack.h"
#include "queue.h"
#include "../common/defines.h"
#include <iostream>

/* ========================= DFS ========================== */

/**
 * @brief DFS 递归访问
 */
static void dfs_visit(const GraphBase* graph,
                      int city_id,
                      bool* visited,
                      int* sequence,
                      int* seq_index)
{
    visited[city_id] = true;
    sequence[(*seq_index)++] = city_id;
    DEBUG_PRINT("DFS 访问顶点 " << city_id);

    Edge_t* neighbors = nullptr;
    int neighbor_count = 0;
    graph->get_neighbors(city_id, &neighbors, &neighbor_count);

    for (int i = 0; i < neighbor_count; ++i) {
        int next = neighbors[i].to;
        if (!visited[next]) {
            dfs_visit(graph, next, visited, sequence, seq_index);
        }
    }

    delete[] neighbors;
}

int traverse_dfs(const GraphBase* graph,
                 int start_city,
                 int** out_sequence,
                 int* out_length)
{
    if (graph == nullptr || out_sequence == nullptr || out_length == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int vertex_count = graph->get_vertex_count();
    if (vertex_count == 0) {
        return ERR_GRAPH_EMPTY;
    }

    if (!graph->has_vertex(start_city)) {
        return ERR_CITY_NOT_FOUND;
    }

    /* 分配 visited 数组和结果数组 */
    int* all_ids = nullptr;
    int all_count = 0;
    graph->get_all_vertex_ids(&all_ids, &all_count);

    bool* visited = new bool[vertex_count]();
    int* sequence = new int[vertex_count];

    /* 建立 city_id → 内部索引的映射 */
    /* visited 用 all_ids 的索引下标 */
    int seq_index = 0;

    /* 从起点开始 DFS */
    dfs_visit(graph, start_city, visited, sequence, &seq_index);

    /* 处理非连通图——遍历所有未访问顶点 */
    for (int i = 0; i < all_count; ++i) {
        if (!visited[all_ids[i]]) {
            dfs_visit(graph, all_ids[i], visited, sequence, &seq_index);
        }
    }

    delete[] visited;
    delete[] all_ids;

    *out_sequence = sequence;
    *out_length = seq_index;
    return SUCCESS;
}

/* ========================= BFS ========================== */

int traverse_bfs(const GraphBase* graph,
                 int start_city,
                 int** out_sequence,
                 int* out_length)
{
    if (graph == nullptr || out_sequence == nullptr || out_length == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int vertex_count = graph->get_vertex_count();
    if (vertex_count == 0) {
        return ERR_GRAPH_EMPTY;
    }

    if (!graph->has_vertex(start_city)) {
        return ERR_CITY_NOT_FOUND;
    }

    int* all_ids = nullptr;
    int all_count = 0;
    graph->get_all_vertex_ids(&all_ids, &all_count);

    bool* visited = new bool[vertex_count]();
    int* sequence = new int[vertex_count];
    int seq_index = 0;

    Queue queue(vertex_count);
    queue.push(start_city);
    visited[start_city] = true;

    while (!queue.empty()) {
        int current = queue.pop();
        DEBUG_PRINT("BFS 出队顶点 " << all_ids[current]);
        sequence[seq_index++] = current;

        Edge_t* neighbors = nullptr;
        int neighbor_count = 0;
        graph->get_neighbors(current, &neighbors, &neighbor_count);

        for (int i = 0; i < neighbor_count; ++i) {
            int next = neighbors[i].to;
            if (!visited[next]) {
                visited[next] = true;
                queue.push(next);
            }
        }

        delete[] neighbors;
    }

    /* 处理非连通图 */
    for (int i = 0; i < all_count; ++i) {
        if (!visited[all_ids[i]]) {
            queue.push(all_ids[i]);
            visited[all_ids[i]] = true;

            while (!queue.empty()) {
                int current = queue.pop();
                sequence[seq_index++] = current;

                Edge_t* neighbors = nullptr;
                int neighbor_count = 0;
                graph->get_neighbors(current, &neighbors, &neighbor_count);

                for (int j = 0; j < neighbor_count; ++j) {
                    int next = neighbors[j].to;
                    if (!visited[next]) {
                        visited[next] = true;
                        queue.push(next);
                    }
                }

                delete[] neighbors;
            }
        }
    }

    delete[] visited;
    delete[] all_ids;

    *out_sequence = sequence;
    *out_length = seq_index;
    return SUCCESS;
}

/* ========================= 格式化输出 ========================== */

void print_traversal_sequence(const GraphBase* graph,
                              const int* sequence,
                              int length,
                              const char* algo_name)
{
    if (graph == nullptr || sequence == nullptr || algo_name == nullptr) {
        return;
    }

    std::cout << "===== " << algo_name << " 遍历序列 =====" << std::endl;
    std::cout << "共 " << length << " 个城市：" << std::endl;

    for (int i = 0; i < length; ++i) {
        City_t city;
        if (graph->get_vertex(sequence[i], &city) == SUCCESS) {
            std::cout << "  [" << i + 1 << "] " << city.name
                      << " (编号: " << city.id << ")" << std::endl;
        }
    }

    std::cout << std::endl;
}
