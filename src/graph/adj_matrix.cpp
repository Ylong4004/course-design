/*
 * 模块名称  : 图存储--邻接矩阵实现
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : AdjMatrix 成员函数实现。适合稠密图，边权查询为 O(1)。
 */

#include "adj_matrix.h"

#include <iostream>

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

    vertices = new City_t[this->max_vertices];
    valid = new bool[this->max_vertices];

    for (int i = 0; i < this->max_vertices; ++i) {
        vertices[i].id = INVALID_ID;
        vertices[i].name[0] = '\0';
        valid[i] = false;
    }
}

AdjMatrix::~AdjMatrix() {
    free_matrix();

    delete[] vertices;
    delete[] valid;
    vertices = nullptr;
    valid = nullptr;
}

StorageType AdjMatrix::get_storage_type() const {
    return STORAGE_MATRIX;
}

const char *AdjMatrix::get_storage_name() const {
    return "邻接矩阵";
}

int AdjMatrix::get_vertex_count() const {
    return vertex_count;
}

int AdjMatrix::get_edge_count() const {
    return edge_count;
}

GraphType AdjMatrix::get_graph_type() const {
    return graph_type;
}

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

bool AdjMatrix::has_vertex(int city_id) const {
    return find_index(city_id) != -1;
}

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

void AdjMatrix::reset_perf_counters() {
    find_comparisons = 0;
    edge_query_count = 0;
}

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

int AdjMatrix::init_matrix() {
    matrix = new int *[max_vertices];

    for (int i = 0; i < max_vertices; ++i) {
        matrix[i] = nullptr;
    }

    for (int i = 0; i < max_vertices; ++i) {
        matrix[i] = new int[max_vertices];

        for (int j = 0; j < max_vertices; ++j) {
            matrix[i][j] = INF_WEIGHT;
        }
    }

    return SUCCESS;
}

void AdjMatrix::free_matrix() {
    if (matrix == nullptr) {
        return;
    }

    for (int i = 0; i < max_vertices; ++i) {
        delete[] matrix[i];
    }

    delete[] matrix;
    matrix = nullptr;
}
