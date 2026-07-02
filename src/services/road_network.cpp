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

/**
 * @brief 将源城市名称安全拷贝到目标缓冲区，自动截断超长字符串并保证以 '\0' 结尾。
 * @param dest_name 目标缓冲区指针
 * @param source_name 源城市名称字符串
 */
static void copy_city_name(char *dest_name, const char *source_name)
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

/**
 * @brief 校验城市输入是否合法（ID 有效且名称非空）。
 * @param city_id 城市ID
 * @param city_name 城市名称
 * @return true 输入合法，false 无效
 */
bool is_valid_city_input(int city_id, const char *city_name)
{
    return city_id != INVALID_ID && city_name != nullptr && city_name[0] != '\0';
}
/* ---- 辅助函数结束 ---- */

/**
 * @brief 构造函数，同时创建邻接矩阵和邻接表两套存储结构。
 */
RoadNetwork::RoadNetwork(int max_vertices, GraphType graph_type)
    : matrix_graph(nullptr),
      list_graph(nullptr),
      graph_type(graph_type),
      max_cities(max_vertices > 0 ? max_vertices : 0)
{
    safe_new(matrix_graph, AdjMatrix, max_cities, graph_type);
    safe_new(list_graph, AdjList, max_cities, graph_type);
}

/**
 * @brief 析构函数，释放矩阵和列表两套存储结构的内存。
 */
RoadNetwork::~RoadNetwork()
{
    safe_delete(matrix_graph);
    safe_delete(list_graph);
}

/**
 * @brief 根据存储类型获取对应的图结构指针（邻接矩阵或邻接表）。
 * @param type 存储类型标识
 * @return 对应的图结构指针，类型无效时返回 nullptr
 */
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

/**
 * @brief 获取路网的图类型。
 */
GraphType RoadNetwork::get_type() const
{
    return graph_type;
}

int RoadNetwork::reset(GraphType new_type)
{
    if (new_type != GRAPH_UNDIRECTED && new_type != GRAPH_DIRECTED)
    {
        return ERR_INVALID_INPUT;
    }

    safe_delete(matrix_graph);
    safe_delete(list_graph);

    graph_type = new_type;
    safe_new(matrix_graph, AdjMatrix, max_cities, graph_type);
    safe_new(list_graph, AdjList, max_cities, graph_type);

    return SUCCESS;
}

/**
 * @brief 向路网中添加一个城市，同步写入邻接矩阵和邻接表两套存储。
 * @param id 城市编号
 * @param name 城市名称
 * @return SUCCESS 成功，ERR_INVALID_INPUT 参数无效，ERR_GRAPH_FULL 路网已满，ERR_CITY_DUPLICATE 城市重复
 */
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

/**
 * @brief 从路网中删除指定城市及其所有关联道路，同步更新两套存储。
 * @param city_id 要删除的城市ID
 * @return SUCCESS 成功，ERR_INVALID_INPUT 路网未初始化，ERR_CITY_NOT_FOUND 城市不存在
 */
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

/**
 * @brief 判断指定城市是否存在于路网中。
 */
bool RoadNetwork::has_city(int city_id) const
{
    return matrix_graph != nullptr && matrix_graph->has_vertex(city_id);
}

/**
 * @brief 根据城市ID查询城市名称。
 * @param city_id 城市ID
 * @param out_name 输出缓冲区，存放城市名称
 * @return SUCCESS 成功，ERR_INVALID_INPUT 参数无效，ERR_CITY_NOT_FOUND 城市不存在
 */
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

/**
 * @brief 获取路网中当前的城市数量。
 */
int RoadNetwork::get_city_count() const
{
    if (matrix_graph == nullptr) {
        return 0;
    }

    return matrix_graph->get_vertex_count();
}

/**
 * @brief 在标准输出中列出路网中所有城市的编号和名称。
 */
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

/**
 * @brief 向路网中添加一条道路，同步写入邻接矩阵和邻接表。
 * @param from 起点城市ID
 * @param to 终点城市ID
 * @param weight 道路权值（必须为正且小于 INF_WEIGHT）
 * @return SUCCESS 成功，ERR_INVALID_INPUT 参数无效，ERR_SELF_LOOP 自环边，ERR_CITY_NOT_FOUND 城市不存在，ERR_ROAD_EXISTS 道路已存在
 */
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

/**
 * @brief 从路网中删除指定道路，同步更新两套存储。
 * @param from 起点城市ID
 * @param to 终点城市ID
 * @return SUCCESS 成功，ERR_INVALID_INPUT 路网未初始化，ERR_ROAD_NOT_FOUND 道路不存在
 */
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

/**
 * @brief 更新指定道路的权值，同步更新两套存储。若列表更新失败则回滚矩阵更新。
 * @param from 起点城市ID
 * @param to 终点城市ID
 * @param new_weight 新权值
 * @return SUCCESS 成功，ERR_INVALID_INPUT 参数无效，ERR_ROAD_NOT_FOUND 道路不存在
 */
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

/**
 * @brief 查询指定道路的权值。
 * @param from 起点城市ID
 * @param to 终点城市ID
 * @return 道路权值，若道路不存在或路网未初始化则返回 INF_WEIGHT
 */
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

/**
 * @brief 判断两城市之间是否存在道路。
 */
bool RoadNetwork::has_road(int from, int to) const
{
    return matrix_graph != nullptr && matrix_graph->has_edge(from, to);
}

/**
 * @brief 获取路网中当前的道路数量。
 */
int RoadNetwork::get_road_count() const
{
    if (matrix_graph == nullptr) {
        return 0;
    }

    return matrix_graph->get_edge_count();
}

/**
 * @brief 打印路网总览信息（图类型、最大容量、当前城市数和道路数）。
 */
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

/**
 * @brief 并排打印邻接矩阵和邻接表两套存储结构的内容。
 */
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

/**
 * @brief 打印路网详细信息，包括全部城市列表和全部道路列表。
 */
void RoadNetwork::print_network_detail() const
{
    if (matrix_graph == nullptr) {
        std::cout << "路网尚未初始化。" << std::endl;
        return;
    }

    int* vertex_ids = nullptr;
    int vertex_count = 0;
    if (matrix_graph->get_all_vertex_ids(&vertex_ids, &vertex_count) != SUCCESS) {
        return;
    }

    /* 打印所有顶点 */
    std::cout << "=== 全部城市（" << vertex_count << " 个）===" << std::endl;
    for (int i = 0; i < vertex_count; ++i) {
        City_t city;
        if (matrix_graph->get_vertex(vertex_ids[i], &city) == SUCCESS) {
            std::cout << "  [" << city.id << "] " << city.name << std::endl;
        }
    }

    /* 打印所有边 */
    int total_edges = 0;
    std::cout << std::endl << "=== 全部道路 ===" << std::endl;
    std::cout << "起点  →  终点    权值" << std::endl;
    std::cout << "-----------------------" << std::endl;

    for (int i = 0; i < vertex_count; ++i) {
        Edge_t* neighbors = nullptr;
        int nb_count = 0;
        if (matrix_graph->get_neighbors(vertex_ids[i], &neighbors, &nb_count) == SUCCESS) {
            for (int j = 0; j < nb_count; ++j) {
                /* 无向图只显示 from < to 的边，避免重复 */
                if (graph_type == GRAPH_UNDIRECTED && vertex_ids[i] > neighbors[j].to) {
                    continue;
                }
                std::cout << " " << neighbors[j].from << "  →  "
                          << neighbors[j].to << "     "
                          << neighbors[j].weight << std::endl;
                ++total_edges;
            }
            delete[] neighbors;
        }
    }
    std::cout << "共 " << total_edges << " 条道路" << std::endl;
    delete[] vertex_ids;
}
