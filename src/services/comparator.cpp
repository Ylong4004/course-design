/*
 * 模块名称  : 服务——结构性能对比
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 邻接矩阵 vs 邻接表 性能横向对比统计。
 *             自动收集两套结构的内存占用、遍历耗时、查找效率等指标。
 */

#include "comparator.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief 在已有城市ID数组中线性查找指定城市的下标。
 * @param city_ids 城市ID数组
 * @param city_count 城市数量
 * @param city_id 要查找的城市ID
 * @return 找到则返回数组下标，否则返回 -1
 */
static int find_city_index(const int *city_ids, int city_count, int city_id)
{
    for (int i = 0; i < city_count; ++i) {
        if (city_ids[i] == city_id) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief 从一个起点出发，用栈模拟DFS遍历一个连通分量的所有顶点。
 * @param graph 图存储结构指针
 * @param city_ids 全部城市ID数组
 * @param city_count 城市数量
 * @param start_index 起点在 city_ids 中的下标
 * @param visited 访问标记数组（会被原地修改）
 */
static void traverse_dfs_component(const GraphBase *graph,
                            const int *city_ids,
                            int city_count,
                            int start_index,
                            std::vector<char> &visited)
{
    std::vector<int> stack;
    stack.push_back(city_ids[start_index]);

    while (!stack.empty()) {
        int current_city = stack.back();
        stack.pop_back();

        int current_index = find_city_index(city_ids, city_count, current_city);
        if (current_index == -1 || visited[static_cast<std::size_t>(current_index)]) {
            continue;
        }

        visited[static_cast<std::size_t>(current_index)] = 1;

        Edge_t *neighbors = nullptr;
        int neighbor_count = 0;
        if (graph->get_neighbors(current_city, &neighbors, &neighbor_count) != SUCCESS) {
            delete[] neighbors;
            continue;
        }

        for (int i = neighbor_count - 1; i >= 0; --i) {
            int next_index = find_city_index(city_ids, city_count, neighbors[i].to);
            if (next_index != -1 && !visited[static_cast<std::size_t>(next_index)]) {
                stack.push_back(neighbors[i].to);
            }
        }

        delete[] neighbors;
    }
}

/**
 * @brief 从一个起点出发，用队列实现BFS遍历一个连通分量的所有顶点。
 * @param graph 图存储结构指针
 * @param city_ids 全部城市ID数组
 * @param city_count 城市数量
 * @param start_index 起点在 city_ids 中的下标
 * @param visited 访问标记数组（会被原地修改）
 */
static void traverse_bfs_component(const GraphBase *graph,
                            const int *city_ids,
                            int city_count,
                            int start_index,
                            std::vector<char> &visited)
{
    std::vector<int> queue;
    std::size_t head = 0;
    queue.push_back(city_ids[start_index]);

    while (head < queue.size()) {
        int current_city = queue[head++];
        int current_index = find_city_index(city_ids, city_count, current_city);
        if (current_index == -1 || visited[static_cast<std::size_t>(current_index)]) {
            continue;
        }

        visited[static_cast<std::size_t>(current_index)] = 1;

        Edge_t *neighbors = nullptr;
        int neighbor_count = 0;
        if (graph->get_neighbors(current_city, &neighbors, &neighbor_count) != SUCCESS) {
            delete[] neighbors;
            continue;
        }

        for (int i = 0; i < neighbor_count; ++i) {
            int next_index = find_city_index(city_ids, city_count, neighbors[i].to);
            if (next_index != -1 && !visited[static_cast<std::size_t>(next_index)]) {
                queue.push_back(neighbors[i].to);
            }
        }

        delete[] neighbors;
    }
}

/**
 * @brief 对图执行一次完整的 DFS + BFS 遍历并计时，返回遍历总耗时（毫秒）。
 * @param graph 图存储结构指针
 * @return 遍历耗时（毫秒），图无效或为空时返回 0.0
 */
double benchmark_traversal(const GraphBase *graph)
{
    if (graph == nullptr) {
        return 0.0;
    }

    int *city_ids = nullptr;
    int city_count = 0;
    if (graph->get_all_vertex_ids(&city_ids, &city_count) != SUCCESS || city_count <= 0) {
        delete[] city_ids;
        return 0.0;
    }

    std::vector<char> visited(static_cast<std::size_t>(city_count), 0);
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < city_count; ++i) {
        if (!visited[static_cast<std::size_t>(i)]) {
            traverse_dfs_component(graph, city_ids, city_count, i, visited);
        }
    }

    for (int i = 0; i < city_count; ++i) {
        visited[static_cast<std::size_t>(i)] = 0;
    }

    for (int i = 0; i < city_count; ++i) {
        if (!visited[static_cast<std::size_t>(i)]) {
            traverse_bfs_component(graph, city_ids, city_count, i, visited);
        }
    }

    auto end = std::chrono::steady_clock::now();
    delete[] city_ids;

    std::chrono::duration<double, std::milli> duration = end - start;
    return duration.count();
}
/* ---- 辅助函数结束 ---- */

/**
 * @brief 构造函数，绑定待对比的邻接矩阵和邻接表两套图结构。
 */
StructureComparator::StructureComparator(GraphBase *matrix_graph,
                                         GraphBase *list_graph)
    : matrix_graph(matrix_graph),
      list_graph(list_graph)
{
}

/**
 * @brief 析构函数，将图指针置空（不负责释放图对象本身）。
 */
StructureComparator::~StructureComparator()
{
    matrix_graph = nullptr;
    list_graph = nullptr;
}

/**
 * @brief 分别测量邻接矩阵和邻接表的内存占用（字节数）。
 * @param out_matrix_bytes 输出参数，邻接矩阵内存占用
 * @param out_list_bytes 输出参数，邻接表内存占用
 */
void StructureComparator::measure_memory(size_t *out_matrix_bytes,
                                         size_t *out_list_bytes) const
{
    if (out_matrix_bytes == nullptr || out_list_bytes == nullptr) {
        return;
    }

    *out_matrix_bytes = 0;
    *out_list_bytes = 0;

    if (matrix_graph != nullptr) {
        PerfStats_t stats{};
        matrix_graph->get_performance_stats(&stats);
        *out_matrix_bytes = stats.memory_bytes;
    }

    if (list_graph != nullptr) {
        PerfStats_t stats{};
        list_graph->get_performance_stats(&stats);
        *out_list_bytes = stats.memory_bytes;
    }
}

/**
 * @brief 分别测量邻接矩阵和邻接表的遍历耗时（毫秒）。
 * @param out_matrix_ms 输出参数，邻接矩阵遍历耗时
 * @param out_list_ms 输出参数，邻接表遍历耗时
 */
void StructureComparator::measure_traverse_time(double *out_matrix_ms,
                                                 double *out_list_ms) const
{
    if (out_matrix_ms == nullptr || out_list_ms == nullptr) {
        return;
    }

    *out_matrix_ms = benchmark_traversal(matrix_graph);
    *out_list_ms = benchmark_traversal(list_graph);
}

/**
 * @brief 分别测量邻接矩阵和邻接表的顶点查找效率（比较次数）。
 * @param out_matrix_cmps 输出参数，邻接矩阵查找比较次数
 * @param out_list_cmps 输出参数，邻接表查找比较次数
 */
void StructureComparator::measure_find_efficiency(int *out_matrix_cmps,
                                                   int *out_list_cmps) const
{
    if (out_matrix_cmps == nullptr || out_list_cmps == nullptr) {
        return;
    }

    *out_matrix_cmps = 0;
    *out_list_cmps = 0;

    const GraphBase *graphs[2] = {matrix_graph, list_graph};
    int *out_values[2] = {out_matrix_cmps, out_list_cmps};

    for (int g = 0; g < 2; ++g) {
        const GraphBase *graph = graphs[g];
        if (graph == nullptr) {
            continue;
        }

        GraphBase *mutable_graph = const_cast<GraphBase *>(graph);
        mutable_graph->reset_perf_counters();

        int *city_ids = nullptr;
        int city_count = 0;
        if (graph->get_all_vertex_ids(&city_ids, &city_count) != SUCCESS) {
            delete[] city_ids;
            continue;
        }

        for (int i = 0; i < city_count; ++i) {
            City_t city{};
            graph->get_vertex(city_ids[i], &city);
        }

        PerfStats_t stats{};
        graph->get_performance_stats(&stats);
        *out_values[g] = stats.find_comparisons;
        delete[] city_ids;
    }
}

/**
 * @brief 分别测量邻接矩阵和邻接表的边查询效率（比较次数）。
 *
 * 对矩阵使用 O(1) 直接访问，对邻接表使用链表遍历。
 * @param out_matrix_cmps 输出参数，邻接矩阵边查询比较次数
 * @param out_list_cmps 输出参数，邻接表边查询比较次数
 */
void StructureComparator::measure_edge_query(int *out_matrix_cmps,
                                             int *out_list_cmps) const
{
    if (out_matrix_cmps == nullptr || out_list_cmps == nullptr) {
        return;
    }

    *out_matrix_cmps = 0;
    *out_list_cmps = 0;

    const GraphBase *graphs[2] = {matrix_graph, list_graph};
    int *out_values[2] = {out_matrix_cmps, out_list_cmps};

    for (int g = 0; g < 2; ++g) {
        const GraphBase *graph = graphs[g];
        if (graph == nullptr) {
            continue;
        }

        int *city_ids = nullptr;
        int city_count = 0;
        if (graph->get_all_vertex_ids(&city_ids, &city_count) != SUCCESS) {
            delete[] city_ids;
            continue;
        }

        int comparisons = 0;
        if (graph->get_storage_type() == STORAGE_MATRIX) {
            for (int i = 0; i < city_count; ++i) {
                for (int j = 0; j < city_count; ++j) {
                    ++comparisons;
                    graph->has_edge(city_ids[i], city_ids[j]);
                }
            }
        } else {
            for (int i = 0; i < city_count; ++i) {
                Edge_t *neighbors = nullptr;
                int neighbor_count = 0;
                if (graph->get_neighbors(city_ids[i], &neighbors, &neighbor_count) != SUCCESS) {
                    delete[] neighbors;
                    continue;
                }

                for (int j = 0; j < city_count; ++j) {
                    for (int k = 0; k < neighbor_count; ++k) {
                        ++comparisons;
                        if (neighbors[k].to == city_ids[j]) {
                            break;
                        }
                    }
                }

                delete[] neighbors;
            }
        }

        *out_values[g] = comparisons;
        delete[] city_ids;
    }
}

/**
 * @brief 运行完整的性能对比，依次测量内存、遍历耗时、查找效率和边查询效率，并输出对比报告。
 */
void StructureComparator::run_full_comparison() const
{
    std::cout << "=== 结构性能对比报告 ===" << std::endl;

    size_t matrix_memory = 0;
    size_t list_memory = 0;
    double matrix_traverse_ms = 0.0;
    double list_traverse_ms = 0.0;
    int matrix_find_cmps = 0;
    int list_find_cmps = 0;
    int matrix_edge_cmps = 0;
    int list_edge_cmps = 0;

    measure_memory(&matrix_memory, &list_memory);
    measure_traverse_time(&matrix_traverse_ms, &list_traverse_ms);
    measure_find_efficiency(&matrix_find_cmps, &list_find_cmps);
    measure_edge_query(&matrix_edge_cmps, &list_edge_cmps);

    std::cout << "内存占用: 矩阵 " << matrix_memory << " B"
              << ", 邻接表 " << list_memory << " B" << std::endl;
    std::cout << "遍历耗时: 矩阵 " << matrix_traverse_ms << " ms"
              << ", 邻接表 " << list_traverse_ms << " ms" << std::endl;
    std::cout << "顶点查找比较次数: 矩阵 " << matrix_find_cmps
              << ", 邻接表 " << list_find_cmps << std::endl;
    std::cout << "边查询比较次数: 矩阵 " << matrix_edge_cmps
              << ", 邻接表 " << list_edge_cmps << std::endl;

    print_conclusion();
}

/**
 * @brief 打印结构性能对比的总结性结论和建议。
 */
void StructureComparator::print_conclusion() const
{
    std::cout << "=== 对比结论 ===" << std::endl;
    std::cout << "邻接矩阵更适合稠密图，边查询更直接；" << std::endl;
    std::cout << "邻接表更适合稀疏图，内存开销更低，遍历邻接点更高效。" << std::endl;
}