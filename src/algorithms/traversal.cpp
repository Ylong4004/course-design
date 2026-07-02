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

static int find_city_index(const int* city_ids, int city_count, int city_id)
{
    if (city_ids == nullptr) {
        return -1;
    }

    for (int i = 0; i < city_count; ++i) {
        if (city_ids[i] == city_id) {
            return i;
        }
    }

    return -1;
}

/* ========================= DFS ========================== */

/**
 * @brief DFS 递归访问
 */
static void dfs_visit(const GraphBase* graph,
                      int city_id,
                      const int* all_ids,
                      int all_count,
                      bool* visited,
                      int* sequence,
                      int* seq_index)
{
    const int city_index = find_city_index(all_ids, all_count, city_id);
    if (city_index < 0 || visited[city_index]) {
        return;
    }

    visited[city_index] = true;
    sequence[(*seq_index)++] = city_id;
    DEBUG_PRINT("DFS 访问顶点 " << city_id);

    Edge_t* neighbors = nullptr;
    int neighbor_count = 0;
    graph->get_neighbors(city_id, &neighbors, &neighbor_count);

    for (int i = 0; i < neighbor_count; ++i) {
        int next = neighbors[i].to;
        int next_index = find_city_index(all_ids, all_count, next);
        if (next_index >= 0 && !visited[next_index]) {
            dfs_visit(graph, next, all_ids, all_count,
                      visited, sequence, seq_index);
        }
    }

    delete[] neighbors;
}

/**
 * @brief 深度优先遍历（DFS），从起点出发递归访问所有可达顶点，并处理非连通图
 * @param graph 图指针
 * @param start_city 遍历起点城市编号
 * @param out_sequence 输出遍历序列（调用者需 delete[]）
 * @param out_length 输出序列长度
 * @return SUCCESS 或错误码
 */
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

    bool* visited = new bool[all_count]();
    int* sequence = new int[all_count];
    int seq_index = 0;

    /* 从起点开始 DFS */
    dfs_visit(graph, start_city, all_ids, all_count,
              visited, sequence, &seq_index);

    /* 处理非连通图——遍历所有未访问顶点 */
    for (int i = 0; i < all_count; ++i) {
        if (!visited[i]) {
            dfs_visit(graph, all_ids[i], all_ids, all_count,
                      visited, sequence, &seq_index);
        }
    }

    delete[] visited;
    delete[] all_ids;

    *out_sequence = sequence;
    *out_length = seq_index;
    return SUCCESS;
}

/* ========================= BFS ========================== */

/**
 * @brief 广度优先遍历（BFS），使用队列逐层访问，并处理非连通图
 * @param graph 图指针
 * @param start_city 遍历起点城市编号
 * @param out_sequence 输出遍历序列（调用者需 delete[]）
 * @param out_length 输出序列长度
 * @return SUCCESS 或错误码
 */
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

    bool* visited = new bool[all_count]();
    int* sequence = new int[all_count];
    int seq_index = 0;

    Queue queue(vertex_count);
    const int start_index = find_city_index(all_ids, all_count, start_city);
    if (start_index < 0) {
        delete[] visited;
        delete[] sequence;
        delete[] all_ids;
        return ERR_CITY_NOT_FOUND;
    }

    queue.push(start_city);
    visited[start_index] = true;

    while (!queue.empty()) {
        int current = queue.pop();
        DEBUG_PRINT("BFS 出队顶点 " << current);
        sequence[seq_index++] = current;

        Edge_t* neighbors = nullptr;
        int neighbor_count = 0;
        graph->get_neighbors(current, &neighbors, &neighbor_count);

        for (int i = 0; i < neighbor_count; ++i) {
            int next = neighbors[i].to;
            int next_index = find_city_index(all_ids, all_count, next);
            if (next_index >= 0 && !visited[next_index]) {
                visited[next_index] = true;
                queue.push(next);
            }
        }

        delete[] neighbors;
    }

    /* 处理非连通图 */
    for (int i = 0; i < all_count; ++i) {
        if (!visited[i]) {
            queue.push(all_ids[i]);
            visited[i] = true;

            while (!queue.empty()) {
                int current = queue.pop();
                sequence[seq_index++] = current;

                Edge_t* neighbors = nullptr;
                int neighbor_count = 0;
                graph->get_neighbors(current, &neighbors, &neighbor_count);

                for (int j = 0; j < neighbor_count; ++j) {
                    int next = neighbors[j].to;
                    int next_index = find_city_index(all_ids, all_count, next);
                    if (next_index >= 0 && !visited[next_index]) {
                        visited[next_index] = true;
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

/**
 * @brief 格式化输出遍历序列，打印城市名称和编号
 * @param graph 图指针
 * @param sequence 遍历序列数组
 * @param length 序列长度
 * @param algo_name 算法名称（如 "DFS"、"BFS"）
 */
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
