/*
 * 模块名称  : 图存储——邻接矩阵实现
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : AdjMatrix 成员函数实现（基于邻接矩阵）
 */

#include "adj_matrix.h"
#include <cstdio>
#include <iostream>

#ifndef SUCCESS
#define SUCCESS 0
#define FAILURE -1
#endif

#ifndef INF_WEIGHT
#define INF_WEIGHT 2147483647
#endif

// ==================== 构造/析构 ====================

AdjMatrix::AdjMatrix(int max_vertices, GraphType graph_type)
    : matrix(nullptr), vertices(nullptr), valid(nullptr),
      vertex_count(0), max_vertices(max_vertices), edge_count(0),
      graph_type(graph_type), find_comparisons(0), edge_query_count(0)
{
    init_matrix();
    vertices = new City_t[max_vertices];
    valid = new bool[max_vertices]();
}

AdjMatrix::~AdjMatrix()
{
    free_matrix();
    delete[] vertices;
    delete[] valid;
    vertices = nullptr;
    valid = nullptr;
}

// ==================== 存储标识 ====================

StorageType AdjMatrix::get_storage_type() const
{
    return STORAGE_MATRIX;
}

const char *AdjMatrix::get_storage_name() const
{
    return "邻接矩阵";
}

// ==================== 图属性查询 ====================

int AdjMatrix::get_vertex_count() const
{
    return vertex_count;
}

int AdjMatrix::get_edge_count() const
{
    return edge_count;
}

GraphType AdjMatrix::get_graph_type() const
{
    return graph_type;
}

int AdjMatrix::get_max_vertex_count() const
{
    return max_vertices;
}

// ==================== 顶点操作 ====================

int AdjMatrix::add_vertex(const City_t &city)
{
    // 检查容量
    if (vertex_count >= max_vertices)
        return FAILURE;

    // 检查重复
    if (find_index(city.id) != -1)
        return FAILURE;

    // 寻找空位
    int idx = -1;
    for (int i = 0; i < max_vertices; ++i) {
        if (!valid[i]) {
            idx = i;
            break;
        }
    }
    if (idx == -1) return FAILURE; // 理论上不会发生

    vertices[idx] = city;
    valid[idx] = true;
    vertex_count++;
    return SUCCESS;
}

int AdjMatrix::remove_vertex(int city_id)
{
    int idx = find_index(city_id);
    if (idx == -1)
        return FAILURE;

    // 清除与该顶点相关的所有边（出边 + 入边）
    for (int i = 0; i < max_vertices; ++i) {
        if (valid[i]) {
            // 出边 matrix[idx][i]
            if (matrix[idx][i] != INF_WEIGHT) {
                matrix[idx][i] = INF_WEIGHT;
                if (graph_type == GRAPH_UNDIRECTED)
                    matrix[i][idx] = INF_WEIGHT;
                edge_count--;
            }
            // 入边 matrix[i][idx]（有向图才需单独处理，避免重复）
            if (graph_type != GRAPH_UNDIRECTED && i != idx) {
                if (matrix[i][idx] != INF_WEIGHT) {
                    matrix[i][idx] = INF_WEIGHT;
                    edge_count--;
                }
            }
        }
    }

    // 标记无效
    valid[idx] = false;
    vertex_count--;
    return SUCCESS;
}

bool AdjMatrix::has_vertex(int city_id) const
{
    return find_index(city_id) != -1;
}

int AdjMatrix::get_vertex(int city_id, City_t *out_city) const
{
    int idx = find_index(city_id);
    if (idx == -1 || out_city == nullptr)
        return FAILURE;

    *out_city = vertices[idx];
    return SUCCESS;
}

// ==================== 边操作 ====================

int AdjMatrix::add_edge(int from, int to, int weight)
{
    int i = find_index(from);
    int j = find_index(to);
    if (i == -1 || j == -1)
        return FAILURE;

    // 边已存在？
    if (matrix[i][j] != INF_WEIGHT)
        return FAILURE;

    matrix[i][j] = weight;
    if (graph_type == GRAPH_UNDIRECTED)
        matrix[j][i] = weight;
    edge_count++;
    return SUCCESS;
}

int AdjMatrix::remove_edge(int from, int to)
{
    int i = find_index(from);
    int j = find_index(to);
    if (i == -1 || j == -1)
        return FAILURE;

    if (matrix[i][j] == INF_WEIGHT)
        return FAILURE;

    matrix[i][j] = INF_WEIGHT;
    if (graph_type == GRAPH_UNDIRECTED)
        matrix[j][i] = INF_WEIGHT;
    edge_count--;
    return SUCCESS;
}

int AdjMatrix::update_edge_weight(int from, int to, int new_weight)
{
    int i = find_index(from);
    int j = find_index(to);
    if (i == -1 || j == -1)
        return FAILURE;

    if (matrix[i][j] == INF_WEIGHT)
        return FAILURE;

    matrix[i][j] = new_weight;
    if (graph_type == GRAPH_UNDIRECTED)
        matrix[j][i] = new_weight;
    return SUCCESS;
}

int AdjMatrix::get_edge_weight(int from, int to, int *out_weight) const
{
    int i = find_index(from);
    int j = find_index(to);
    if (i == -1 || j == -1 || out_weight == nullptr)
        return FAILURE;

    if (matrix[i][j] == INF_WEIGHT)
        return FAILURE;

    *out_weight = matrix[i][j];
    return SUCCESS;
}

bool AdjMatrix::has_edge(int from, int to) const
{
    int i = find_index(from);
    int j = find_index(to);
    if (i == -1 || j == -1)
        return false;
    return matrix[i][j] != INF_WEIGHT;
}

// ==================== 遍历支持 ====================

int AdjMatrix::get_neighbors(int vertex_id,
                             Edge_t **out_neighbors,
                             int *out_count) const
{
    int idx = find_index(vertex_id);
    if (idx == -1 || out_neighbors == nullptr || out_count == nullptr)
        return FAILURE;

    // 统计邻接顶点数
    int count = 0;
    for (int j = 0; j < max_vertices; ++j) {
        if (valid[j] && matrix[idx][j] != INF_WEIGHT)
            count++;
    }

    if (count == 0) {
        *out_neighbors = nullptr;
        *out_count = 0;
        return SUCCESS;
    }

    Edge_t *neighbors = new Edge_t[count];
    int pos = 0;
    for (int j = 0; j < max_vertices; ++j) {
        if (valid[j] && matrix[idx][j] != INF_WEIGHT) {
            neighbors[pos].dest_city_id = vertices[j].id;
            neighbors[pos].weight = matrix[idx][j];
            pos++;
        }
    }

    *out_neighbors = neighbors;
    *out_count = count;
    return SUCCESS;
}

int AdjMatrix::get_all_vertex_ids(int **out_ids, int *out_count) const
{
    if (out_ids == nullptr || out_count == nullptr)
        return FAILURE;

    if (vertex_count == 0) {
        *out_ids = nullptr;
        *out_count = 0;
        return SUCCESS;
    }

    int *ids = new int[vertex_count];
    int pos = 0;
    for (int i = 0; i < max_vertices; ++i) {
        if (valid[i])
            ids[pos++] = vertices[i].id;
    }

    *out_ids = ids;
    *out_count = vertex_count;
    return SUCCESS;
}

// ==================== 性能统计 ====================

void AdjMatrix::get_performance_stats(PerfStats_t *out_stats) const
{
    if (out_stats == nullptr) return;

    // 矩阵内存：行指针数组 + 每行数据
    int matrix_mem = max_vertices * sizeof(int*) + max_vertices * max_vertices * sizeof(int);
    int vertices_mem = max_vertices * sizeof(City_t);
    int valid_mem = max_vertices * sizeof(bool);

    out_stats->memory_bytes = matrix_mem + vertices_mem + valid_mem;
    out_stats->find_comparisons = find_comparisons;
    out_stats->edge_query_count = edge_query_count;
    out_stats->traverse_time_us = 0; // 可后续填充
}

void AdjMatrix::reset_perf_counters()
{
    find_comparisons = 0;
    edge_query_count = 0;
}

// ==================== 调试输出 ====================

void AdjMatrix::print_graph() const
{
    std::cout << "=== 邻接矩阵 ===" << std::endl;
    std::cout << "顶点数: " << vertex_count << "  边数: " << edge_count << std::endl;

    // 打印列头（仅有效顶点）
    printf("     ");
    for (int j = 0; j < max_vertices; ++j) {
        if (valid[j])
            printf("%4d ", vertices[j].id);
    }
    printf("\n");

    // 逐行打印
    for (int i = 0; i < max_vertices; ++i) {
        if (!valid[i]) continue;
        printf("%4d ", vertices[i].id);
        for (int j = 0; j < max_vertices; ++j) {
            if (!valid[j]) continue;
            if (matrix[i][j] == INF_WEIGHT)
                printf("  *  ");
            else
                printf("%4d ", matrix[i][j]);
        }
        printf("\n");
    }
    std::cout << "==================" << std::endl;
}

// ==================== 私有方法 ====================

int AdjMatrix::find_index(int city_id) const
{
    find_comparisons++;
    for (int i = 0; i < max_vertices; ++i) {
        if (valid[i] && vertices[i].id == city_id)
            return i;
    }
    return -1;
}

int AdjMatrix::init_matrix()
{
    matrix = new int*[max_vertices];
    for (int i = 0; i < max_vertices; ++i) {
        matrix[i] = new int[max_vertices];
        for (int j = 0; j < max_vertices; ++j)
            matrix[i][j] = INF_WEIGHT;
    }
    return SUCCESS;
}

void AdjMatrix::free_matrix()
{
    if (matrix != nullptr) {
        for (int i = 0; i < max_vertices; ++i)
            delete[] matrix[i];
        delete[] matrix;
        matrix = nullptr;
    }
}