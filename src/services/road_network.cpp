/*
 * 模块名称  : 服务——路网管理
 * 编写人    : 组员A（架构负责人） + 组员B（核心功能）
 * 功能描述  : 路网构建、编辑、查询等业务操作。
 *             同时管理邻接矩阵和邻接表两套存储，保证数据同步。
 */

#include "road_network.h"

#include "../common/defines.h"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <new>

namespace
{
void copy_city_name(char *dest_name, const char *source_name)
{
    if (dest_name == nullptr) {
        return;
    }

    if (source_name == nullptr) {
        dest_name[0] = '\0';
        return;
    }

    std::strncpy(dest_name, source_name, MAX_CITY_NAME - 1);
    dest_name[MAX_CITY_NAME - 1] = '\0';
}

bool is_valid_city_input(int city_id, const char *city_name)
{
    return city_id != INVALID_ID && city_name != nullptr && city_name[0] != '\0';
}
} // namespace

RoadNetwork::RoadNetwork(int max_vertices, GraphType graph_type)
    : matrix_graph(nullptr),
      list_graph(nullptr),
      graph_type(graph_type),
      max_cities(max_vertices > 0 ? max_vertices : 0)
{
    safe_new(matrix_graph, AdjMatrix, max_cities, graph_type);
    safe_new(list_graph, AdjList, max_cities, graph_type);
}

RoadNetwork::~RoadNetwork()
{
    delete matrix_graph;
    delete list_graph;
    matrix_graph = nullptr;
    list_graph = nullptr;
}

GraphBase *RoadNetwork::get_graph(StorageType type)
{
    switch (type) {
    case STORAGE_MATRIX:
        return matrix_graph;
    case STORAGE_LIST:
        return list_graph;
    default:
        return nullptr;
    }
}

GraphType RoadNetwork::get_type() const
{
    return graph_type;
}

int RoadNetwork::add_city(int id, const char *name)
{
    if (!is_valid_city_input(id, name) || matrix_graph == nullptr || list_graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    if (std::strlen(name) >= MAX_CITY_NAME) {
        return ERR_INVALID_INPUT;
    }

    City_t city{};
    city.id = id;
    copy_city_name(city.name, name);

    int ret = matrix_graph->add_vertex(city);
    if (ret != SUCCESS) {
        return ret;
    }

    ret = list_graph->add_vertex(city);
    if (ret != SUCCESS) {
        matrix_graph->remove_vertex(id);
        return ret;
    }

    return SUCCESS;
}

int RoadNetwork::remove_city(int city_id)
{
    if (matrix_graph == nullptr || list_graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    if (!has_city(city_id)) {
        return ERR_CITY_NOT_FOUND;
    }

    int ret = matrix_graph->remove_vertex(city_id);
    if (ret != SUCCESS) {
        return ret;
    }

    ret = list_graph->remove_vertex(city_id);
    if (ret != SUCCESS) {
        return ret;
    }

    return SUCCESS;
}

bool RoadNetwork::has_city(int city_id) const
{
    return matrix_graph != nullptr && matrix_graph->has_vertex(city_id);
}

int RoadNetwork::get_city_name(int city_id, char *out_name) const
{
    if (out_name == nullptr || matrix_graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    City_t city{};
    int ret = matrix_graph->get_vertex(city_id, &city);
    if (ret != SUCCESS) {
        return ret;
    }

    copy_city_name(out_name, city.name);
    return SUCCESS;
}

int RoadNetwork::get_city_count() const
{
    if (matrix_graph == nullptr) {
        return 0;
    }

    return matrix_graph->get_vertex_count();
}

void RoadNetwork::list_all_cities() const
{
    if (matrix_graph == nullptr) {
        std::cout << "当前路网尚未初始化。" << std::endl;
        return;
    }

    int *city_ids = nullptr;
    int city_count = 0;
    int ret = matrix_graph->get_all_vertex_ids(&city_ids, &city_count);
    if (ret != SUCCESS) {
        std::cout << "无法读取城市列表。" << std::endl;
        return;
    }

    std::cout << "=== 城市列表 ===" << std::endl;
    if (city_count == 0) {
        std::cout << "暂无城市数据。" << std::endl;
        delete[] city_ids;
        return;
    }

    for (int i = 0; i < city_count; ++i) {
        City_t city{};
        if (matrix_graph->get_vertex(city_ids[i], &city) == SUCCESS) {
            std::cout << city.id << "  " << city.name << std::endl;
        }
    }

    delete[] city_ids;
}

int RoadNetwork::add_road(int from, int to, int weight)
{
    if (matrix_graph == nullptr || list_graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    if (from == to) {
        return ERR_SELF_LOOP;
    }

    if (weight <= 0 || weight >= INF_WEIGHT) {
        return ERR_INVALID_INPUT;
    }

    if (!has_city(from) || !has_city(to)) {
        return ERR_CITY_NOT_FOUND;
    }

    int ret = matrix_graph->add_edge(from, to, weight);
    if (ret != SUCCESS) {
        return ret;
    }

    ret = list_graph->add_edge(from, to, weight);
    if (ret != SUCCESS) {
        matrix_graph->remove_edge(from, to);
        return ret;
    }

    return SUCCESS;
}

int RoadNetwork::remove_road(int from, int to)
{
    if (matrix_graph == nullptr || list_graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    if (!has_road(from, to)) {
        return ERR_ROAD_NOT_FOUND;
    }

    int ret = matrix_graph->remove_edge(from, to);
    if (ret != SUCCESS) {
        return ret;
    }

    ret = list_graph->remove_edge(from, to);
    if (ret != SUCCESS) {
        return ret;
    }

    return SUCCESS;
}

int RoadNetwork::update_road_weight(int from, int to, int new_weight)
{
    if (matrix_graph == nullptr || list_graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    if (new_weight <= 0 || new_weight >= INF_WEIGHT) {
        return ERR_INVALID_INPUT;
    }

    int original_weight = 0;
    int ret = matrix_graph->get_edge_weight(from, to, &original_weight);
    if (ret != SUCCESS) {
        return ret;
    }

    ret = matrix_graph->update_edge_weight(from, to, new_weight);
    if (ret != SUCCESS) {
        return ret;
    }

    ret = list_graph->update_edge_weight(from, to, new_weight);
    if (ret != SUCCESS) {
        matrix_graph->update_edge_weight(from, to, original_weight);
        return ret;
    }

    return SUCCESS;
}

int RoadNetwork::get_road_weight(int from, int to) const
{
    if (matrix_graph == nullptr) {
        return INF_WEIGHT;
    }

    int weight = INF_WEIGHT;
    if (matrix_graph->get_edge_weight(from, to, &weight) != SUCCESS) {
        return INF_WEIGHT;
    }

    return weight;
}

bool RoadNetwork::has_road(int from, int to) const
{
    return matrix_graph != nullptr && matrix_graph->has_edge(from, to);
}

int RoadNetwork::get_road_count() const
{
    if (matrix_graph == nullptr) {
        return 0;
    }

    return matrix_graph->get_edge_count();
}

void RoadNetwork::print_network_overview() const
{
    if (matrix_graph == nullptr || list_graph == nullptr) {
        std::cout << "路网尚未初始化。" << std::endl;
        return;
    }

    std::cout << "=== 路网总览 ===" << std::endl;
    std::cout << "图类型: "
              << (graph_type == GRAPH_DIRECTED ? "有向图" : "无向图")
              << std::endl;
    std::cout << "最大城市数: " << max_cities << std::endl;
    std::cout << "当前城市数: " << get_city_count() << std::endl;
    std::cout << "当前道路数: " << get_road_count() << std::endl;
}

void RoadNetwork::print_both_structures() const
{
    if (matrix_graph == nullptr || list_graph == nullptr) {
        std::cout << "路网尚未初始化。" << std::endl;
        return;
    }

    std::cout << "=== 邻接矩阵 ===" << std::endl;
    matrix_graph->print_graph();
    std::cout << std::endl;
    std::cout << "=== 邻接表 ===" << std::endl;
    list_graph->print_graph();
}