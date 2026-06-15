/*
 * 模块名称  : 图存储--邻接表实现
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : AdjList 成员函数实现。适合稀疏图，邻接遍历效率高。
 */

#include "adj_list.h"

#include <cstring>
#include <iostream>

/**
 * @brief 判断邻接表头结点槽位是否被有效城市占用。
 *
 * AdjListHead_t 没有额外 valid 字段，因此用 city.id == INVALID_ID
 * 表示空槽位。这是本实现和头文件结构体之间的约定。
 */
bool is_valid_head(const AdjListHead_t &head) {
    return head.city.id != INVALID_ID;
}

/**
 * @brief 从链表中删除一条指向 dest_city 的边。
 * @return 找到并删除返回 true，否则返回 false。
 */
bool unlink_edge(EdgeNode_t **head, int dest_city) {
    if (head == nullptr) {
        return false;
    }

    EdgeNode_t *prev = nullptr;
    EdgeNode_t *curr = *head;

    while (curr != nullptr) {
        if (curr->dest_city == dest_city) {
            if (prev == nullptr) {
                *head = curr->next;
            } else {
                prev->next = curr->next;
            }
            delete curr;
            return true;
        }

        prev = curr;
        curr = curr->next;
    }

    return false;
}

/**
 * @brief 统计链表节点数量。
 */
int count_edge_nodes(EdgeNode_t *head) {
    int count = 0;

    while (head != nullptr) {
        ++count;
        head = head->next;
    }

    return count;
}

AdjList::AdjList(int max_vertices, GraphType graph_type)
    : heads(nullptr),
      vertex_count(0),
      max_vertices(max_vertices > 0 ? max_vertices : 0),
      edge_count(0),
      graph_type(graph_type),
      find_comparisons(0),
      edge_query_count(0) {
    if (this->max_vertices == 0) {
        return;
    }

    heads = new AdjListHead_t[this->max_vertices];

    for (int i = 0; i < this->max_vertices; ++i) {
        heads[i].city.id = INVALID_ID;
        heads[i].city.name[0] = '\0';
        heads[i].first_edge = nullptr;
    }
}

AdjList::~AdjList() {
    if (heads != nullptr) {
        for (int i = 0; i < max_vertices; ++i) {
            free_edge_list(i);
        }
    }

    delete[] heads;
    heads = nullptr;
}

StorageType AdjList::get_storage_type() const {
    return STORAGE_LIST;
}

const char *AdjList::get_storage_name() const {
    return "邻接表";
}

int AdjList::get_vertex_count() const {
    return vertex_count;
}

int AdjList::get_edge_count() const {
    return edge_count;
}

GraphType AdjList::get_graph_type() const {
    return graph_type;
}

int AdjList::get_max_vertex_count() const {
    return max_vertices;
}

/**
 * @brief 添加城市顶点。
 */
int AdjList::add_vertex(const City_t &city) {
    if (heads == nullptr || city.id == INVALID_ID) {
        return ERR_INVALID_INPUT;
    }

    if (vertex_count >= max_vertices) {
        return ERR_GRAPH_FULL;
    }

    if (find_index(city.id) != -1) {
        return ERR_CITY_DUPLICATE;
    }

    for (int i = 0; i < max_vertices; ++i) {
        if (!is_valid_head(heads[i])) {
            heads[i].city = city;
            heads[i].first_edge = nullptr;
            ++vertex_count;
            return SUCCESS;
        }
    }

    return ERR_GRAPH_FULL;
}

/**
 * @brief 删除城市及其所有关联边。
 */
int AdjList::remove_vertex(int city_id) {
    int index = find_index(city_id);
    if (index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    if (graph_type == GRAPH_UNDIRECTED) {
        // 无向图的每条逻辑边在链表中存两份，只按该点度数扣一次。
        edge_count -= count_edge_nodes(heads[index].first_edge);
        free_edge_list(index);

        for (int i = 0; i < max_vertices; ++i) {
            if (i != index && is_valid_head(heads[i])) {
                unlink_edge(&heads[i].first_edge, city_id);
            }
        }
    } else {
        // 有向图需分别扣除出边和入边。
        edge_count -= count_edge_nodes(heads[index].first_edge);
        free_edge_list(index);

        for (int i = 0; i < max_vertices; ++i) {
            if (i != index && is_valid_head(heads[i])) {
                if (unlink_edge(&heads[i].first_edge, city_id)) {
                    --edge_count;
                }
            }
        }
    }

    heads[index].city.id = INVALID_ID;
    heads[index].city.name[0] = '\0';
    heads[index].first_edge = nullptr;
    --vertex_count;

    return SUCCESS;
}

bool AdjList::has_vertex(int city_id) const {
    return find_index(city_id) != -1;
}

int AdjList::get_vertex(int city_id, City_t *out_city) const {
    if (out_city == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int index = find_index(city_id);
    if (index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    *out_city = heads[index].city;
    return SUCCESS;
}

/**
 * @brief 添加道路边。无向图会同步写入反向链表节点。
 */
int AdjList::add_edge(int from, int to, int weight) {
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

    if (find_edge_node(from, to) != nullptr) {
        return ERR_ROAD_EXISTS;
    }

    EdgeNode_t *forward = create_edge_node(to, weight);
    if (forward == nullptr) {
        return ERR_OUT_OF_MEMORY;
    }

    forward->next = heads[from_index].first_edge;
    heads[from_index].first_edge = forward;

    if (graph_type == GRAPH_UNDIRECTED) {
        EdgeNode_t *backward = create_edge_node(from, weight);
        if (backward == nullptr) {
            unlink_edge(&heads[from_index].first_edge, to);
            return ERR_OUT_OF_MEMORY;
        }

        backward->next = heads[to_index].first_edge;
        heads[to_index].first_edge = backward;
    }

    ++edge_count;
    return SUCCESS;
}

int AdjList::remove_edge(int from, int to) {
    int from_index = find_index(from);
    int to_index = find_index(to);
    if (from_index == -1 || to_index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    if (!unlink_edge(&heads[from_index].first_edge, to)) {
        return ERR_ROAD_NOT_FOUND;
    }

    if (graph_type == GRAPH_UNDIRECTED) {
        unlink_edge(&heads[to_index].first_edge, from);
    }

    --edge_count;
    return SUCCESS;
}

int AdjList::update_edge_weight(int from, int to, int new_weight) {
    if (new_weight < 0) {
        return ERR_INVALID_INPUT;
    }

    EdgeNode_t *edge = find_edge_node(from, to);
    if (edge == nullptr) {
        return ERR_ROAD_NOT_FOUND;
    }

    edge->weight = new_weight;

    if (graph_type == GRAPH_UNDIRECTED) {
        EdgeNode_t *reverse = find_edge_node(to, from);
        if (reverse != nullptr) {
            reverse->weight = new_weight;
        }
    }

    return SUCCESS;
}

int AdjList::get_edge_weight(int from, int to, int *out_weight) const {
    if (out_weight == nullptr) {
        return ERR_INVALID_INPUT;
    }

    EdgeNode_t *edge = find_edge_node(from, to);
    if (edge == nullptr) {
        return ERR_ROAD_NOT_FOUND;
    }

    *out_weight = edge->weight;
    return SUCCESS;
}

bool AdjList::has_edge(int from, int to) const {
    return find_edge_node(from, to) != nullptr;
}

/**
 * @brief 获取指定城市的所有邻接边。
 *
 * 调用者负责 delete[] *out_neighbors。
 */
int AdjList::get_neighbors(int vertex_id,
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

    int count = count_edge_nodes(heads[index].first_edge);
    if (count == 0) {
        return SUCCESS;
    }

    Edge_t *neighbors = new Edge_t[count];

    EdgeNode_t *curr = heads[index].first_edge;
    for (int i = 0; i < count; ++i) {
        neighbors[i].from = vertex_id;
        neighbors[i].to = curr->dest_city;
        neighbors[i].weight = curr->weight;
        curr = curr->next;
    }

    *out_neighbors = neighbors;
    *out_count = count;
    return SUCCESS;
}

int AdjList::get_all_vertex_ids(int **out_ids, int *out_count) const {
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
        if (is_valid_head(heads[i])) {
            ids[pos++] = heads[i].city.id;
        }
    }

    *out_ids = ids;
    *out_count = vertex_count;
    return SUCCESS;
}

void AdjList::get_performance_stats(PerfStats_t *out_stats) const {
    if (out_stats == nullptr) {
        return;
    }

    size_t memory_bytes = sizeof(AdjList);
    memory_bytes += static_cast<size_t>(max_vertices) * sizeof(AdjListHead_t);

    for (int i = 0; i < max_vertices; ++i) {
        EdgeNode_t *curr = heads[i].first_edge;
        while (curr != nullptr) {
            memory_bytes += sizeof(EdgeNode_t);
            curr = curr->next;
        }
    }

    out_stats->memory_bytes = memory_bytes;
    out_stats->traverse_time_ms = 0.0;
    out_stats->find_comparisons = find_comparisons;
    out_stats->edge_query_count = edge_query_count;
}

void AdjList::reset_perf_counters() {
    find_comparisons = 0;
    edge_query_count = 0;
}

void AdjList::print_graph() const {
    std::cout << "=== Adjacency List ===" << std::endl;
    std::cout << "vertices: " << vertex_count
              << ", edges: " << edge_count << std::endl;

    for (int i = 0; i < max_vertices; ++i) {
        if (!is_valid_head(heads[i])) {
            continue;
        }

        std::cout << "[" << heads[i].city.id << " "
                  << heads[i].city.name << "]";

        EdgeNode_t *curr = heads[i].first_edge;
        if (curr == nullptr) {
            std::cout << " -> (none)";
        }

        while (curr != nullptr) {
            std::cout << " -> (" << curr->dest_city
                      << ", w=" << curr->weight << ")";
            curr = curr->next;
        }

        std::cout << std::endl;
    }

    std::cout << "======================" << std::endl;
}

int AdjList::find_index(int city_id) const {
    if (heads == nullptr) {
        return -1;
    }

    for (int i = 0; i < max_vertices; ++i) {
        ++find_comparisons;
        if (is_valid_head(heads[i]) && heads[i].city.id == city_id) {
            return i;
        }
    }

    return -1;
}

EdgeNode_t *AdjList::find_edge_node(int from, int to) const {
    ++edge_query_count;

    int from_index = find_index(from);
    if (from_index == -1) {
        return nullptr;
    }

    EdgeNode_t *curr = heads[from_index].first_edge;
    while (curr != nullptr) {
        if (curr->dest_city == to) {
            return curr;
        }
        curr = curr->next;
    }

    return nullptr;
}

void AdjList::free_edge_list(int index) {
    if (heads == nullptr || index < 0 || index >= max_vertices) {
        return;
    }

    EdgeNode_t *curr = heads[index].first_edge;
    while (curr != nullptr) {
        EdgeNode_t *next = curr->next;
        delete curr;
        curr = next;
    }

    heads[index].first_edge = nullptr;
}

EdgeNode_t *AdjList::create_edge_node(int dest_city, int weight) const {
    EdgeNode_t *node = new EdgeNode_t;

    node->dest_city = dest_city;
    node->weight = weight;
    node->next = nullptr;
    return node;
}
