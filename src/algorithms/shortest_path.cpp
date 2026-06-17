/*
 * 模块名称  : 算法——最短路径（Dijkstra & Floyd）
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : Dijkstra 单源最短路径 + Floyd 多源最短路径的实现。
 */

#include "shortest_path.h"
#include "../common/defines.h"
#include <iostream>
#include <iomanip>
#include <climits>

/* ========================= Dijkstra ========================== */

/**
 * @brief Dijkstra 单源最短路径算法，计算从起点到所有顶点的最短距离
 * @param graph 图指针
 * @param start_city 起点城市编号
 * @param out_dist 输出距离数组（以 city_id 为下标）
 * @param out_prev 输出前驱数组（以 city_id 为下标）
 * @return SUCCESS 或错误码
 */
int run_dijkstra(const GraphBase* graph,
                 int start_city,
                 int* out_dist,
                 int* out_prev)
{
    if (graph == nullptr || out_dist == nullptr || out_prev == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int vertex_count = graph->get_vertex_count();
    if (vertex_count == 0) {
        return ERR_GRAPH_EMPTY;
    }

    if (!graph->has_vertex(start_city)) {
        return ERR_CITY_NOT_FOUND;
    }

    /* 获取所有顶点列表 */
    int* all_ids = nullptr;
    int all_count = 0;
    graph->get_all_vertex_ids(&all_ids, &all_count);

    /* 建立 city_id → 内部下标 的映射 */
    const int max_vertices = graph->get_max_vertex_count();
    int* id_to_index = new int[max_vertices];
    for (int i = 0; i < max_vertices; ++i) {
        id_to_index[i] = -1;
    }
    for (int i = 0; i < all_count; ++i) {
        id_to_index[all_ids[i]] = i;
    }

    /* 初始化 */
    int n = all_count;
    bool* determined = new bool[n]();
    for (int i = 0; i < n; ++i) {
        out_dist[i] = INF_WEIGHT;
        out_prev[i] = -1;
    }

    int start_idx = id_to_index[start_city];
    if (start_idx < 0) {
        delete[] id_to_index;
        delete[] determined;
        delete[] all_ids;
        return ERR_CITY_NOT_FOUND;
    }

    out_dist[start_idx] = 0;

    /* 主循环：每次确定一个顶点的最短路径 */
    for (int count = 0; count < n; ++count) {
        /* 从未确定顶点中选取 dist 最小的 */
        int min_dist = INF_WEIGHT;
        int u = -1;

        for (int i = 0; i < n; ++i) {
            if (!determined[i] && out_dist[i] < min_dist) {
                min_dist = out_dist[i];
                u = i;
            }
        }

        /* 剩余顶点不可达 */
        if (u == -1) {
            break;
        }

        determined[u] = true;
        DEBUG_PRINT("Dijkstra 选中顶点 " << all_ids[u] << "，距离=" << min_dist);

        /* 松弛操作 */
        int city_id = all_ids[u];
        Edge_t* neighbors = nullptr;
        int neighbor_count = 0;
        graph->get_neighbors(city_id, &neighbors, &neighbor_count);

        for (int i = 0; i < neighbor_count; ++i) {
            int v = id_to_index[neighbors[i].to];
            int weight = neighbors[i].weight;

            if (v >= 0 && !determined[v] &&
                out_dist[u] != INF_WEIGHT &&
                out_dist[u] + weight < out_dist[v]) {
                out_dist[v] = out_dist[u] + weight;
                DEBUG_PRINT("  松弛: " << all_ids[v] << " 距离更新为 " << out_dist[v]);
                out_prev[v] = city_id;
            }
        }

        delete[] neighbors;
    }

    delete[] id_to_index;
    delete[] determined;
    delete[] all_ids;

    return SUCCESS;
}

/**
 * @brief 根据 Dijkstra 前驱数组回溯出起点到终点的具体路径
 * @param prev 前驱数组（以 city_id 为下标）
 * @param vertex_count 顶点总数
 * @param start_city 起点城市编号
 * @param end_city 终点城市编号
 * @param out_path 输出路径数组（调用者需 delete[]），不可达时为 nullptr
 * @param out_len 输出路径长度
 * @return SUCCESS、ALGO_NO_PATH 或错误码
 */
int dijkstra_get_path(const int* prev,
                      int vertex_count,
                      int start_city,
                      int end_city,
                      int** out_path,
                      int* out_len)
{
    if (prev == nullptr || out_path == nullptr || out_len == nullptr) {
        return ERR_INVALID_INPUT;
    }

    /* 先回溯收集路径（倒序） */
    int* temp = new int[vertex_count];
    int len = 0;
    int current = end_city;

    while (current != -1 && current != start_city) {
        temp[len++] = current;

        /* 找 current 的前驱 */
        /* prev 数组是内部的，current 是 city_id */
        /* 这里 prev 存储的是前驱 city_id，直接使用 */
        current = prev[current];  /* 注意：prev 用 city_id 作为下标 */

        if (len > vertex_count) {
            /* 死循环保护 */
            delete[] temp;
            return ERR_HAS_CYCLE;
        }
    }

    if (current == -1) {
        /* 不可达 */
        delete[] temp;
        *out_path = nullptr;
        *out_len = 0;
        return ALGO_NO_PATH;
    }

    /* 加入起点，翻转顺序 */
    *out_len = len + 1;
    *out_path = new int[*out_len];
    (*out_path)[0] = start_city;
    for (int i = 0; i < len; ++i) {
        (*out_path)[len - i] = temp[i];
    }

    delete[] temp;
    return SUCCESS;
}

/**
 * @brief 格式化打印 Dijkstra 单源最短路径结果表格
 * @param graph 图指针
 * @param start_city 起点城市编号
 * @param dist 距离数组
 * @param prev 前驱数组
 */
void print_dijkstra_result(const GraphBase* graph,
                           int start_city,
                           const int* dist,
                           const int* prev)
{
    if (graph == nullptr || dist == nullptr || prev == nullptr) {
        return;
    }

    int* all_ids = nullptr;
    int all_count = 0;
    graph->get_all_vertex_ids(&all_ids, &all_count);

    City_t start_info;
    graph->get_vertex(start_city, &start_info);

    std::cout << "===== Dijkstra 单源最短路径 =====" << std::endl;
    std::cout << "起点: " << start_info.name << " (编号: " << start_city << ")"
              << std::endl;
    std::cout << std::endl;

    std::cout << std::left << std::setw(20) << "目标城市"
              << std::right << std::setw(12) << "最短距离"
              << std::setw(12) << "最短路径(顶点数)"
              << std::endl;
    std::cout << std::string(44, '-') << std::endl;

    for (int i = 0; i < all_count; ++i) {
        int city_id = all_ids[i];
        if (city_id == start_city) {
            continue;
        }

        City_t city_info;
        graph->get_vertex(city_id, &city_info);

        if (dist[city_id] == INF_WEIGHT) {
            std::cout << std::left << std::setw(20) << city_info.name
                      << std::right << std::setw(12) << "不可达"
                      << std::setw(12) << "-"
                      << std::endl;
        } else {
            int* path = nullptr;
            int path_len = 0;
            dijkstra_get_path(prev, all_count, start_city, city_id,
                              &path, &path_len);

            std::cout << std::left << std::setw(20) << city_info.name
                      << std::right << std::setw(12) << dist[city_id]
                      << std::setw(12) << path_len
                      << std::endl;

            delete[] path;
        }
    }

    delete[] all_ids;
    std::cout << std::endl;
}

/* ========================= Floyd ========================== */

/**
 * @brief Floyd 多源最短路径算法，计算所有顶点对之间的最短距离
 * @param graph 图指针
 * @param out_dist 输出距离矩阵（调用者需逐行和整体 delete[]）
 * @param out_next 输出路径后继矩阵（调用者需逐行和整体 delete[]）
 * @param vertex_count 输出顶点数量
 * @return SUCCESS 或错误码
 */
int run_floyd(const GraphBase* graph,
              int*** out_dist,
              int*** out_next,
              int* vertex_count)
{
    if (graph == nullptr || out_dist == nullptr ||
        out_next == nullptr || vertex_count == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int* all_ids = nullptr;
    int all_count = 0;
    graph->get_all_vertex_ids(&all_ids, &all_count);

    if (all_count == 0) {
        delete[] all_ids;
        return ERR_GRAPH_EMPTY;
    }

    int n = all_count;

    /* 建立 city_id → 内部下标 映射 */
    const int max_id = graph->get_max_vertex_count();
    int* id_to_index = new int[max_id];
    for (int i = 0; i < max_id; ++i) {
        id_to_index[i] = -1;
    }
    for (int i = 0; i < n; ++i) {
        id_to_index[all_ids[i]] = i;
    }

    /* 分配距离矩阵和路径矩阵 */
    int** dist = new int*[n];
    int** next = new int*[n];
    for (int i = 0; i < n; ++i) {
        dist[i] = new int[n];
        next[i] = new int[n];
    }

    /* 初始化 */
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                dist[i][j] = 0;
                next[i][j] = all_ids[j];
            } else {
                int weight = 0;
                if (graph->get_edge_weight(all_ids[i], all_ids[j], &weight)
                    == SUCCESS) {
                    dist[i][j] = weight;
                    next[i][j] = all_ids[j];
                } else {
                    dist[i][j] = INF_WEIGHT;
                    next[i][j] = -1;
                }
            }
        }
    }

    /* 三重循环：以 k 为中转点 */
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (dist[i][k] != INF_WEIGHT &&
                    dist[k][j] != INF_WEIGHT &&
                    dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }

    delete[] id_to_index;
    delete[] all_ids;

    *out_dist = dist;
    *out_next = next;
    *vertex_count = n;
    return SUCCESS;
}

/**
 * @brief 根据 Floyd 后继矩阵还原 from 到 to 的具体路径
 * @param next 后继矩阵（以 city_id 为行/列下标）
 * @param vertex_count 顶点总数
 * @param from 起点城市编号
 * @param to 终点城市编号
 * @param out_path 输出路径数组（调用者需 delete[]），不可达时为 nullptr
 * @param out_len 输出路径长度
 * @return SUCCESS、ALGO_NO_PATH 或错误码
 */
int floyd_get_path(int** next,
                   int vertex_count,
                   int from, int to,
                   int** out_path,
                   int* out_len)
{
    if (next == nullptr || out_path == nullptr || out_len == nullptr) {
        return ERR_INVALID_INPUT;
    }

    /* 需要 id_to_index 映射才能使用 next 矩阵 */
    /* 但这里 next 的行/列下标就是顶点编号（city_id），直接使用 */
    if (from < 0 || to < 0 || next[from][to] == -1) {
        *out_path = nullptr;
        *out_len = 0;
        return ALGO_NO_PATH;
    }

    int* path = new int[vertex_count];
    int len = 0;
    int current = from;
    path[len++] = current;

    while (current != to && len < vertex_count) {
        current = next[current][to];
        path[len++] = current;
        if (current == -1) {
            delete[] path;
            *out_path = nullptr;
            *out_len = 0;
            return ALGO_NO_PATH;
        }
    }

    *out_path = path;
    *out_len = len;
    return SUCCESS;
}

/**
 * @brief 格式化打印 Floyd 多源最短路径距离对照表
 * @param graph 图指针
 * @param dist 距离矩阵
 * @param next 后继矩阵
 * @param vertex_count 顶点数量
 */
void print_floyd_table(const GraphBase* graph,
                       int** dist,
                       int** next,
                       int vertex_count)
{
    if (graph == nullptr || dist == nullptr || next == nullptr) {
        return;
    }

    std::cout << "===== Floyd 多源最短路径对照表 =====" << std::endl;
    std::cout << "顶点数: " << vertex_count << std::endl;
    std::cout << std::endl;

    /* 表头 */
    std::cout << std::setw(10) << "from\\to";
    for (int j = 0; j < vertex_count; ++j) {
        City_t city;
        graph->get_vertex(j, &city);
        std::cout << std::setw(10) << city.name;
    }
    std::cout << std::endl;
    std::cout << std::string(10 + vertex_count * 10, '-') << std::endl;

    /* 每行 */
    for (int i = 0; i < vertex_count; ++i) {
        City_t city;
        graph->get_vertex(i, &city);
        std::cout << std::setw(10) << city.name;

        for (int j = 0; j < vertex_count; ++j) {
            if (dist[i][j] == INF_WEIGHT) {
                std::cout << std::setw(10) << "INF";
            } else {
                std::cout << std::setw(10) << dist[i][j];
            }
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
}
