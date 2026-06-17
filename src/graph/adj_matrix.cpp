/*
 * 模块名称  : 图存储--邻接矩阵实现
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : AdjMatrix 成员函数实现。适合稠密图，边权查询为 O(1)。
 */

#include "adj_matrix.h"
#include "../common/defines.h"

#include <iostream>

/**
 * @brief 构造函数，初始化邻接矩阵存储结构，分配顶点数组和矩阵内存。
 */
AdjMatrix::AdjMatrix(int max_vertices, GraphType graph_type)
    : matrix(nullptr),
      vertices(nullptr),
      valid(nullptr),
      vertex_count(0),
      max_vertices(max_vertices > 0 ? max_vertices : 0),
      edge_count(0),
      graph_type(graph_type),
      find_comparisons(0),
      edge_query_count(0) {
    if (this->max_vertices == 0) {
        return;
    }

    if (init_matrix() != SUCCESS) {
        this->max_vertices = 0;
        return;
    }

    safe_new_array(vertices, City_t, this->max_vertices);
    safe_new_array(valid, bool, this->max_vertices);

    for (int i = 0; i < this->max_vertices; ++i) {
        vertices[i].id = INVALID_ID;
        vertices[i].name[0] = '\0';
        valid[i] = false;
    }
}

/**
 * @brief 析构函数，释放邻接矩阵及顶点数组的所有动态内存。
 */
AdjMatrix::~AdjMatrix() {
    free_matrix();

    safe_delete_array(vertices);
    safe_delete_array(valid);
}

/**
 * @brief 返回当前存储结构的类型标识（邻接矩阵）。
 */
StorageType AdjMatrix::get_storage_type() const {
    return STORAGE_MATRIX;
}

/**
 * @brief 返回存储结构的中文名称。
 */
const char *AdjMatrix::get_storage_name() const {
    return "邻接矩阵";
}

/**
 * @brief 获取当前图中的顶点数量。
 */
int AdjMatrix::get_vertex_count() const {
    return vertex_count;
}

/**
 * @brief 获取当前图中的边数量。
 */
int AdjMatrix::get_edge_count() const {
    return edge_count;
}

/**
 * @brief 获取图的类型（有向图或无向图）。
 */
GraphType AdjMatrix::get_graph_type() const {
    return graph_type;
}

/**
 * @brief 获取图的最大顶点容量。
 */
int AdjMatrix::get_max_vertex_count() const {
    return max_vertices;
}

/**
 * @brief 添加城市顶点。
 */
int AdjMatrix::add_vertex(const City_t &city) {
    if (matrix == nullptr || vertices == nullptr || valid == nullptr ||
        city.id == INVALID_ID) {
        return ERR_INVALID_INPUT;
    }

    if (vertex_count >= max_vertices) {
        return ERR_GRAPH_FULL;
    }

    if (find_index(city.id) != -1) {
        return ERR_CITY_DUPLICATE;
    }

    for (int i = 0; i < max_vertices; ++i) {
        if (!valid[i]) {
            vertices[i] = city;
            valid[i] = true;
            ++vertex_count;
            return SUCCESS;
        }
    }

    return ERR_GRAPH_FULL;
}

/**
 * @brief 删除城市顶点，并清除所有相关边。
 */
int AdjMatrix::remove_vertex(int city_id) {
    int index = find_index(city_id);
    if (index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    if (graph_type == GRAPH_UNDIRECTED) {
        // 无向图矩阵是对称的，只按一行中的邻接项扣逻辑边数。
        for (int j = 0; j < max_vertices; ++j) {
            if (valid[j] && matrix[index][j] != INF_WEIGHT) {
                matrix[index][j] = INF_WEIGHT;
                matrix[j][index] = INF_WEIGHT;
                --edge_count;
            }
        }
    } else {
        for (int j = 0; j < max_vertices; ++j) {
            if (valid[j] && matrix[index][j] != INF_WEIGHT) {
                matrix[index][j] = INF_WEIGHT;
                --edge_count;
            }

            if (valid[j] && matrix[j][index] != INF_WEIGHT) {
                matrix[j][index] = INF_WEIGHT;
                --edge_count;
            }
        }
    }

    valid[index] = false;
    vertices[index].id = INVALID_ID;
    vertices[index].name[0] = '\0';
    --vertex_count;

    return SUCCESS;
}

/**
 * @brief 判断指定城市ID是否存在于图中。
 */
bool AdjMatrix::has_vertex(int city_id) const {
    return find_index(city_id) != -1;
}

/**
 * @brief 根据城市ID查询城市信息。
 * @param city_id 要查询的城市ID
 * @param out_city 输出参数，存放查询到的城市信息
 * @return SUCCESS 成功，ERR_CITY_NOT_FOUND 城市不存在，ERR_INVALID_INPUT 参数无效
 */
int AdjMatrix::get_vertex(int city_id, City_t *out_city) const {
    if (out_city == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int index = find_index(city_id);
    if (index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    *out_city = vertices[index];
    return SUCCESS;
}

/**
 * @brief 添加道路边。无向图会同步写入对称位置。
 */
int AdjMatrix::add_edge(int from, int to, int weight) {
    if (from == to) {
        return ERR_SELF_LOOP;
    }

    if (weight < 0) {
        return ERR_INVALID_INPUT;
    }

    int from_index = find_index(from);
    int to_index = find_index(to);
    if (from_index == -1 || to_index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    ++edge_query_count;
    if (matrix[from_index][to_index] != INF_WEIGHT) {
        return ERR_ROAD_EXISTS;
    }

    matrix[from_index][to_index] = weight;
    if (graph_type == GRAPH_UNDIRECTED) {
        matrix[to_index][from_index] = weight;
    }

    ++edge_count;
    return SUCCESS;
}

/**
 * @brief 删除图中指定两点之间的道路边。无向图会同时清除对称位置。
 * @param from 起点城市ID
 * @param to 终点城市ID
 * @return SUCCESS 成功，ERR_CITY_NOT_FOUND 城市不存在，ERR_ROAD_NOT_FOUND 边不存在
 */
int AdjMatrix::remove_edge(int from, int to) {
    int from_index = find_index(from);
    int to_index = find_index(to);
    if (from_index == -1 || to_index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    ++edge_query_count;
    if (matrix[from_index][to_index] == INF_WEIGHT) {
        return ERR_ROAD_NOT_FOUND;
    }

    matrix[from_index][to_index] = INF_WEIGHT;
    if (graph_type == GRAPH_UNDIRECTED) {
        matrix[to_index][from_index] = INF_WEIGHT;
    }

    --edge_count;
    return SUCCESS;
}

/**
 * @brief 更新指定道路边的权值。无向图会同步更新对称位置。
 * @param from 起点城市ID
 * @param to 终点城市ID
 * @param new_weight 新的权值（必须非负）
 * @return SUCCESS 成功，ERR_CITY_NOT_FOUND 城市不存在，ERR_ROAD_NOT_FOUND 边不存在，ERR_INVALID_INPUT 权值无效
 */
int AdjMatrix::update_edge_weight(int from, int to, int new_weight) {
    if (new_weight < 0) {
        return ERR_INVALID_INPUT;
    }

    int from_index = find_index(from);
    int to_index = find_index(to);
    if (from_index == -1 || to_index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    ++edge_query_count;
    if (matrix[from_index][to_index] == INF_WEIGHT) {
        return ERR_ROAD_NOT_FOUND;
    }

    matrix[from_index][to_index] = new_weight;
    if (graph_type == GRAPH_UNDIRECTED) {
        matrix[to_index][from_index] = new_weight;
    }

    return SUCCESS;
}

/**
 * @brief 查询指定道路边的权值。
 * @param from 起点城市ID
 * @param to 终点城市ID
 * @param out_weight 输出参数，存放边的权值
 * @return SUCCESS 成功，ERR_CITY_NOT_FOUND 城市不存在，ERR_ROAD_NOT_FOUND 边不存在，ERR_INVALID_INPUT 参数无效
 */
int AdjMatrix::get_edge_weight(int from, int to, int *out_weight) const {
    if (out_weight == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int from_index = find_index(from);
    int to_index = find_index(to);
    if (from_index == -1 || to_index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    ++edge_query_count;
    if (matrix[from_index][to_index] == INF_WEIGHT) {
        return ERR_ROAD_NOT_FOUND;
    }

    *out_weight = matrix[from_index][to_index];
    return SUCCESS;
}

/**
 * @brief 判断两城市之间是否存在道路边。
 */
bool AdjMatrix::has_edge(int from, int to) const {
    int from_index = find_index(from);
    int to_index = find_index(to);
    if (from_index == -1 || to_index == -1) {
        return false;
    }

    ++edge_query_count;
    return matrix[from_index][to_index] != INF_WEIGHT;
}

/**
 * @brief 获取指定城市的所有邻接边。
 *
 * 调用者负责 delete[] *out_neighbors。
 */
int AdjMatrix::get_neighbors(int vertex_id,
                             Edge_t **out_neighbors,
                             int *out_count) const {
    if (out_neighbors == nullptr || out_count == nullptr) {
        return ERR_INVALID_INPUT;
    }

    *out_neighbors = nullptr;
    *out_count = 0;

    int index = find_index(vertex_id);
    if (index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    int count = 0;
    for (int j = 0; j < max_vertices; ++j) {
        if (valid[j] && matrix[index][j] != INF_WEIGHT) {
            ++count;
        }
    }

    if (count == 0) {
        return SUCCESS;
    }

    Edge_t *neighbors = new Edge_t[count];

    int pos = 0;
    for (int j = 0; j < max_vertices; ++j) {
        if (valid[j] && matrix[index][j] != INF_WEIGHT) {
            neighbors[pos].from = vertex_id;
            neighbors[pos].to = vertices[j].id;
            neighbors[pos].weight = matrix[index][j];
            ++pos;
        }
    }

    *out_neighbors = neighbors;
    *out_count = count;
    return SUCCESS;
}

/**
 * @brief 获取图中所有城市ID的列表。
 *
 * 调用者负责 delete[] *out_ids。
 * @param out_ids 输出参数，存放城市ID数组
 * @param out_count 输出参数，存放城市数量
 * @return SUCCESS 成功，ERR_INVALID_INPUT 参数无效
 */
int AdjMatrix::get_all_vertex_ids(int **out_ids, int *out_count) const {
    if (out_ids == nullptr || out_count == nullptr) {
        return ERR_INVALID_INPUT;
    }

    *out_ids = nullptr;
    *out_count = 0;

    if (vertex_count == 0) {
        return SUCCESS;
    }

    int *ids = new int[vertex_count];

    int pos = 0;
    for (int i = 0; i < max_vertices; ++i) {
        if (valid[i]) {
            ids[pos++] = vertices[i].id;
        }
    }

    *out_ids = ids;
    *out_count = vertex_count;
    return SUCCESS;
}

/**
 * @brief 收集当前结构的性能统计数据（内存占用、查找比较次数、边查询次数）。
 * @param out_stats 输出参数，存放性能统计数据
 */
void AdjMatrix::get_performance_stats(PerfStats_t *out_stats) const {
    if (out_stats == nullptr) {
        return;
    }

    size_t matrix_bytes = sizeof(int *) * static_cast<size_t>(max_vertices);
    matrix_bytes += sizeof(int) * static_cast<size_t>(max_vertices) *
                    static_cast<size_t>(max_vertices);

    out_stats->memory_bytes = sizeof(AdjMatrix) + matrix_bytes;
    out_stats->memory_bytes += sizeof(City_t) *
                               static_cast<size_t>(max_vertices);
    out_stats->memory_bytes += sizeof(bool) *
                               static_cast<size_t>(max_vertices);
    out_stats->traverse_time_ms = 0.0;
    out_stats->find_comparisons = find_comparisons;
    out_stats->edge_query_count = edge_query_count;
}

/**
 * @brief 重置所有性能计数器（查找比较次数和边查询次数归零）。
 */
void AdjMatrix::reset_perf_counters() {
    find_comparisons = 0;
    edge_query_count = 0;
}

/**
 * @brief 以表格形式打印邻接矩阵的全部内容到标准输出。
 */
void AdjMatrix::print_graph() const {
    std::cout << "=== Adjacency Matrix ===" << std::endl;
    std::cout << "vertices: " << vertex_count
              << ", edges: " << edge_count << std::endl;

    std::cout << "      ";
    for (int j = 0; j < max_vertices; ++j) {
        if (valid[j]) {
            std::cout << vertices[j].id << "\t";
        }
    }
    std::cout << std::endl;

    for (int i = 0; i < max_vertices; ++i) {
        if (!valid[i]) {
            continue;
        }

        std::cout << vertices[i].id << "\t";
        for (int j = 0; j < max_vertices; ++j) {
            if (!valid[j]) {
                continue;
            }

            if (matrix[i][j] == INF_WEIGHT) {
                std::cout << "*\t";
            } else {
                std::cout << matrix[i][j] << "\t";
            }
        }
        std::cout << std::endl;
    }

    std::cout << "========================" << std::endl;
}

/**
 * @brief 根据城市ID在顶点数组中线性查找其下标。
 * @param city_id 要查找的城市ID
 * @return 找到则返回数组下标，否则返回 -1
 */
int AdjMatrix::find_index(int city_id) const {
    if (valid == nullptr || vertices == nullptr) {
        return -1;
    }

    for (int i = 0; i < max_vertices; ++i) {
        ++find_comparisons;
        if (valid[i] && vertices[i].id == city_id) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief 分配邻接矩阵二维数组内存，并将所有元素初始化为 INF_WEIGHT。
 * @return SUCCESS 成功
 */
int AdjMatrix::init_matrix() {
    matrix = new int *[max_vertices];

    for (int i = 0; i < max_vertices; ++i) {
        matrix[i] = nullptr;
    }

    for (int i = 0; i < max_vertices; ++i) {
        safe_new_array(matrix[i], int, max_vertices);

        for (int j = 0; j < max_vertices; ++j) {
            matrix[i][j] = INF_WEIGHT;
        }
    }

    return SUCCESS;
}

/**
 * @brief 释放邻接矩阵二维数组的全部动态内存。
 */
void AdjMatrix::free_matrix() {
    if (matrix == nullptr) {
        return;
    }

    for (int i = 0; i < max_vertices; ++i) {
        safe_delete_array(matrix[i]);
    }

    safe_delete_array(matrix);
}
