/*
 * 模块名称  : 服务——文件读写与数据持久化
 * 编写人    : 组员A（架构负责人，基础接口） + 组员C（异常完善）
 * 功能描述  : 路网数据的 TXT 文件导入、保存、开机自动加载。
 */

#include "file_io.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

char FileManager::default_path[256] = "data/road_network.txt";

namespace
{
void copy_city_name(char *dest_name, const std::string &source_name)
{
    if (dest_name == nullptr) {
        return;
    }

    std::size_t index = 0;
    for (; index + 1 < MAX_CITY_NAME && index < source_name.size(); ++index) {
        dest_name[index] = source_name[index];
    }
    dest_name[index] = '\0';
}

int find_city_index(const std::vector<City_t> &cities, int city_id)
{
    for (std::size_t i = 0; i < cities.size(); ++i) {
        if (cities[i].id == city_id) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

bool city_exists(const std::vector<City_t> &cities, int city_id)
{
    return find_city_index(cities, city_id) != -1;
}

bool edge_exists(const std::vector<Edge_t> &edges,
                 int from, int to, GraphType graph_type)
{
    int normalized_from = from;
    int normalized_to = to;

    if (graph_type == GRAPH_UNDIRECTED && normalized_from > normalized_to) {
        const int temp = normalized_from;
        normalized_from = normalized_to;
        normalized_to = temp;
    }

    for (std::size_t i = 0; i < edges.size(); ++i) {
        int current_from = edges[i].from;
        int current_to = edges[i].to;

        if (graph_type == GRAPH_UNDIRECTED && current_from > current_to) {
            const int temp = current_from;
            current_from = current_to;
            current_to = temp;
        }

        if (current_from == normalized_from && current_to == normalized_to) {
            return true;
        }
    }

    return false;
}

int clear_graph(GraphBase *graph)
{
    if (graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int *vertex_ids = nullptr;
    int vertex_count = 0;
    int ret = graph->get_all_vertex_ids(&vertex_ids, &vertex_count);
    if (ret != SUCCESS) {
        return ret;
    }

    for (int i = 0; i < vertex_count; ++i) {
        graph->remove_vertex(vertex_ids[i]);
    }

    delete[] vertex_ids;
    return SUCCESS;
}
} // namespace

int FileManager::save_to_file(const GraphBase *graph,
                              const char *filepath)
{
    if (graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    const char *path = (filepath != nullptr && filepath[0] != '\0') ? filepath : default_path;
    std::ofstream outfile(path, std::ios::out | std::ios::trunc);
    if (!outfile.is_open()) {
        return ERR_FILE_OPEN_FAIL;
    }

    int *vertex_ids = nullptr;
    int vertex_count = 0;
    int ret = graph->get_all_vertex_ids(&vertex_ids, &vertex_count);
    if (ret != SUCCESS) {
        return ret;
    }

    outfile << static_cast<int>(graph->get_graph_type()) << ' '
            << vertex_count << ' '
            << graph->get_edge_count() << '\n';

    for (int i = 0; i < vertex_count; ++i) {
        City_t city{};
        ret = graph->get_vertex(vertex_ids[i], &city);
        if (ret != SUCCESS) {
            delete[] vertex_ids;
            return ret;
        }

        outfile << city.id << ' ' << city.name << '\n';
    }

    for (int i = 0; i < vertex_count; ++i) {
        for (int j = 0; j < vertex_count; ++j) {
            if (graph->get_graph_type() == GRAPH_UNDIRECTED && i >= j) {
                continue;
            }

            int weight = INF_WEIGHT;
            if (graph->get_edge_weight(vertex_ids[i], vertex_ids[j], &weight) != SUCCESS) {
                continue;
            }

            outfile << vertex_ids[i] << ' '
                    << vertex_ids[j] << ' '
                    << weight << '\n';
        }
    }

    delete[] vertex_ids;
    return SUCCESS;
}

int FileManager::load_from_file(GraphBase *graph,
                                const char *filepath)
{
    if (graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    const char *path = (filepath != nullptr && filepath[0] != '\0') ? filepath : default_path;
    std::ifstream infile(path, std::ios::in);
    if (!infile.is_open()) {
        return ERR_FILE_OPEN_FAIL;
    }

    int file_graph_type = 0;
    int vertex_count = 0;
    int edge_count = 0;
    if (!(infile >> file_graph_type >> vertex_count >> edge_count)) {
        return ERR_FILE_FORMAT;
    }

    if (file_graph_type != GRAPH_UNDIRECTED && file_graph_type != GRAPH_DIRECTED) {
        return ERR_FILE_FORMAT;
    }

    if (graph->get_graph_type() != static_cast<GraphType>(file_graph_type)) {
        return ERR_FILE_FORMAT;
    }

    if (vertex_count < 0 || edge_count < 0) {
        return ERR_FILE_FORMAT;
    }

    if (vertex_count > graph->get_max_vertex_count()) {
        return ERR_GRAPH_FULL;
    }

    std::vector<City_t> cities;
    std::vector<Edge_t> edges;
    cities.reserve(static_cast<std::size_t>(vertex_count));
    edges.reserve(static_cast<std::size_t>(edge_count));

    for (int i = 0; i < vertex_count; ++i) {
        City_t city{};
        std::string city_name;
        if (!(infile >> city.id >> city_name)) {
            return ERR_FILE_FORMAT;
        }

        if (city.id == INVALID_ID || city_name.empty() || city_name.size() >= MAX_CITY_NAME) {
            return ERR_FILE_FORMAT;
        }

        if (find_city_index(cities, city.id) != -1) {
            return ERR_FILE_FORMAT;
        }

        copy_city_name(city.name, city_name);
        cities.push_back(city);
    }

    for (int i = 0; i < edge_count; ++i) {
        Edge_t edge{};
        if (!(infile >> edge.from >> edge.to >> edge.weight)) {
            return ERR_FILE_FORMAT;
        }

        if (edge.from == edge.to || edge.weight < 0) {
            return ERR_FILE_FORMAT;
        }

        if (!city_exists(cities, edge.from) || !city_exists(cities, edge.to)) {
            return ERR_FILE_FORMAT;
        }

        if (edge_exists(edges, edge.from, edge.to,
                        static_cast<GraphType>(file_graph_type))) {
            return ERR_FILE_FORMAT;
        }

        edges.push_back(edge);
    }

    std::string trailing_token;
    if (infile >> trailing_token) {
        return ERR_FILE_FORMAT;
    }

    int ret = clear_graph(graph);
    if (ret != SUCCESS) {
        return ret;
    }

    for (std::size_t i = 0; i < cities.size(); ++i) {
        ret = graph->add_vertex(cities[i]);
        if (ret != SUCCESS) {
            return ret;
        }
    }

    for (std::size_t i = 0; i < edges.size(); ++i) {
        ret = graph->add_edge(edges[i].from, edges[i].to, edges[i].weight);
        if (ret != SUCCESS) {
            return ret;
        }
    }

    return SUCCESS;
}

int FileManager::auto_load(GraphBase *graph)
{
    return load_from_file(graph, nullptr);
}

bool FileManager::data_file_exists()
{
    std::ifstream infile(default_path, std::ios::in);
    return infile.is_open();
}

void FileManager::set_default_path(const char *path)
{
    const char *source = (path != nullptr && path[0] != '\0') ? path : "data/road_network.txt";
    std::size_t index = 0;
    for (; index + 1 < sizeof(default_path) && source[index] != '\0'; ++index) {
        default_path[index] = source[index];
    }
    default_path[index] = '\0';
}