/*
 * 模块名称  : 服务——拥堵模拟
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 交通拥堵仿真模拟——修改道路权重，对比拥堵前后路径变化。
 */

#include "congestion.h"
#include "../common/defines.h"

#include <iostream>
#include <string>
#include <vector>

/**
 * @brief 在已排序或未排序的城市ID数组中线性查找指定城市的下标。
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
 * @brief 将源数组的内容逐元素拷贝到目标数组。
 * @param dest 目标数组
 * @param source 源数组
 * @param length 要拷贝的元素个数
 */
static void copy_array(int *dest, const int *source, int length)
{
    if (dest == nullptr || source == nullptr || length <= 0) {
        return;
    }

    for (int i = 0; i < length; ++i) {
        dest[i] = source[i];
    }
}

/**
 * @brief 在指定图上运行 Dijkstra 最短路径算法，计算从起点到所有城市的最短距离和前驱节点。
 * @param graph 图存储结构指针
 * @param start_city 起点城市ID
 * @param city_ids 全部城市ID数组
 * @param city_count 城市数量
 * @param out_dist 输出数组，存放最短距离，长度至少为 city_count
 * @param out_prev 输出数组，存放前驱城市ID，长度至少为 city_count
 * @return SUCCESS 成功，ERR_INVALID_INPUT 参数无效，ERR_CITY_NOT_FOUND 起点不存在
 */
static int run_dijkstra_core(const GraphBase *graph,
                      int start_city,
                      const int *city_ids,
                      int city_count,
                      int *out_dist,
                      int *out_prev)
{
    if (graph == nullptr || city_ids == nullptr || out_dist == nullptr || out_prev == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int start_index = find_city_index(city_ids, city_count, start_city);
    if (start_index == -1) {
        return ERR_CITY_NOT_FOUND;
    }

    for (int i = 0; i < city_count; ++i) {
        out_dist[i] = INF_WEIGHT;
        out_prev[i] = INVALID_ID;
    }

    std::vector<char> visited(static_cast<std::size_t>(city_count), 0);
    out_dist[start_index] = 0;

    for (int step = 0; step < city_count; ++step) {
        int current_index = -1;
        int current_dist = INF_WEIGHT;

        for (int i = 0; i < city_count; ++i) {
            if (!visited[static_cast<std::size_t>(i)] && out_dist[i] < current_dist) {
                current_dist = out_dist[i];
                current_index = i;
            }
        }

        if (current_index == -1) {
            break;
        }

        visited[static_cast<std::size_t>(current_index)] = 1;

        Edge_t *neighbors = nullptr;
        int neighbor_count = 0;
        int ret = graph->get_neighbors(city_ids[current_index], &neighbors, &neighbor_count);
        if (ret != SUCCESS) {
            delete[] neighbors;
            return ret;
        }

        for (int i = 0; i < neighbor_count; ++i) {
            int next_index = find_city_index(city_ids, city_count, neighbors[i].to);
            if (next_index == -1 || visited[static_cast<std::size_t>(next_index)]) {
                continue;
            }

            if (out_dist[current_index] == INF_WEIGHT) {
                continue;
            }

            long long candidate = static_cast<long long>(out_dist[current_index]) +
                                  static_cast<long long>(neighbors[i].weight);
            if (candidate < out_dist[next_index]) {
                out_dist[next_index] = static_cast<int>(candidate);
                out_prev[next_index] = city_ids[current_index];
            }
        }

        delete[] neighbors;
    }

    return SUCCESS;
}
/* ---- 辅助函数结束 ---- */

/**
 * @brief 构造函数，初始化拥堵模拟器并分配记录数组。
 */
CongestionSimulator::CongestionSimulator(GraphBase *graph, int max_modify)
    : graph(graph),
      modified_from(nullptr),
      modified_to(nullptr),
      original_weight(nullptr),
      modify_count(0),
      max_modify(max_modify > 0 ? max_modify : 0),
      before_dist(nullptr),
      before_prev(nullptr),
      after_dist(nullptr),
      after_prev(nullptr),
      analysis_city_count(0)
{
    if (this->max_modify > 0) {
        safe_new_array(modified_from, int, this->max_modify);
        safe_new_array(modified_to, int, this->max_modify);
        safe_new_array(original_weight, int, this->max_modify);
    }
}

/**
 * @brief 析构函数，释放所有动态分配的数组内存。
 */
CongestionSimulator::~CongestionSimulator()
{
    delete[] modified_from;
    delete[] modified_to;
    delete[] original_weight;
    delete[] before_dist;
    delete[] before_prev;
    delete[] after_dist;
    delete[] after_prev;

    modified_from = nullptr;
    modified_to = nullptr;
    original_weight = nullptr;
    before_dist = nullptr;
    before_prev = nullptr;
    after_dist = nullptr;
    after_prev = nullptr;
}

/**
 * @brief 设置指定道路的拥堵权值，记录原始权值供后续恢复。
 * @param from 起点城市ID
 * @param to 终点城市ID
 * @param congestion_weight 拥堵后的新权值
 * @return SUCCESS 成功，ERR_INVALID_INPUT 参数无效，ERR_ROAD_NOT_FOUND 道路不存在，ERR_GRAPH_FULL 修改记录已满
 */
int CongestionSimulator::set_congestion(int from, int to, int congestion_weight)
{
    if (graph == nullptr || from == to || congestion_weight <= 0 || congestion_weight >= INF_WEIGHT) {
        return ERR_INVALID_INPUT;
    }

    int original = 0;
    int ret = graph->get_edge_weight(from, to, &original);
    if (ret != SUCCESS) {
        return ret;
    }

    int existing_index = -1;
    for (int i = 0; i < modify_count; ++i) {
        if (modified_from[i] == from && modified_to[i] == to) {
            existing_index = i;
            break;
        }
    }

    if (existing_index == -1) {
        if (modify_count >= max_modify || modified_from == nullptr || modified_to == nullptr || original_weight == nullptr) {
            return ERR_GRAPH_FULL;
        }

        modified_from[modify_count] = from;
        modified_to[modify_count] = to;
        original_weight[modify_count] = original;
        ++modify_count;
    }

    ret = graph->update_edge_weight(from, to, congestion_weight);
    if (ret != SUCCESS) {
        if (existing_index == -1) {
            --modify_count;
        }
        return ret;
    }

    return SUCCESS;
}

/**
 * @brief 将所有已修改的道路权值恢复到原始值，并清空修改记录。
 * @return 第一个遇到的成功/错误码，全部成功则返回 SUCCESS
 */
int CongestionSimulator::restore_all()
{
    if (graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int first_error = SUCCESS;
    for (int i = 0; i < modify_count; ++i) {
        int ret = graph->update_edge_weight(modified_from[i], modified_to[i], original_weight[i]);
        if (ret != SUCCESS && first_error == SUCCESS) {
            first_error = ret;
        }
    }

    modify_count = 0;
    return first_error;
}

/**
 * @brief 打印当前所有已修改道路的详细信息（原始权值与当前权值对比）。
 */
void CongestionSimulator::list_modified_roads() const
{
    std::cout << "=== 已修改道路列表 ===" << std::endl;
    if (modify_count == 0) {
        std::cout << "当前没有拥堵记录。" << std::endl;
        return;
    }

    for (int i = 0; i < modify_count; ++i) {
        int current_weight = INF_WEIGHT;
        int query_ret = graph == nullptr ? ERR_INVALID_INPUT : graph->get_edge_weight(modified_from[i], modified_to[i], &current_weight);

        std::cout << modified_from[i] << " -> " << modified_to[i]
                  << " | 原始权值: " << original_weight[i]
                  << " | 当前权值: ";
        if (query_ret == SUCCESS) {
            std::cout << current_weight;
        } else {
            std::cout << "不可用";
        }
        std::cout << std::endl;
    }
}

/**
 * @brief 运行拥堵前后对比分析：分别在原始权值和拥堵权值下运行 Dijkstra 算法，记录两次结果。
 * @param start_city 起点城市ID
 * @return SUCCESS 成功，ERR_INVALID_INPUT 路网未初始化，ERR_GRAPH_EMPTY 路网为空，ERR_OUT_OF_MEMORY 内存不足
 */
int CongestionSimulator::run_comparison(int start_city)
{
    if (graph == nullptr) {
        return ERR_INVALID_INPUT;
    }

    int *city_ids = nullptr;
    int city_count = 0;
    int ret = graph->get_all_vertex_ids(&city_ids, &city_count);
    if (ret != SUCCESS) {
        return ret;
    }

    if (city_count <= 0) {
        delete[] city_ids;
        return ERR_GRAPH_EMPTY;
    }

    if (analysis_city_count != city_count) {
        delete[] before_dist;
        delete[] before_prev;
        delete[] after_dist;
        delete[] after_prev;

        safe_new_array(before_dist, int, city_count);
        safe_new_array(before_prev, int, city_count);
        safe_new_array(after_dist, int, city_count);
        safe_new_array(after_prev, int, city_count);

        if (before_dist == nullptr || before_prev == nullptr || after_dist == nullptr || after_prev == nullptr) {
            delete[] before_dist;
            delete[] before_prev;
            delete[] after_dist;
            delete[] after_prev;
            before_dist = nullptr;
            before_prev = nullptr;
            after_dist = nullptr;
            after_prev = nullptr;
            delete[] city_ids;
            return ERR_OUT_OF_MEMORY;
        }

        analysis_city_count = city_count;
    }

    std::vector<int> current_weights;
    current_weights.reserve(static_cast<std::size_t>(modify_count));

    for (int i = 0; i < modify_count; ++i) {
        int current_weight = original_weight[i];
        if (graph->get_edge_weight(modified_from[i], modified_to[i], &current_weight) != SUCCESS) {
            current_weights.push_back(original_weight[i]);
        } else {
            current_weights.push_back(current_weight);
        }
    }

    if (modify_count > 0) {
        ret = restore_all();
        if (ret != SUCCESS) {
            delete[] city_ids;
            return ret;
        }
    }

    ret = run_dijkstra_core(graph, start_city, city_ids, city_count, before_dist, before_prev);
    if (ret != SUCCESS) {
        for (int i = 0; i < modify_count && i < static_cast<int>(current_weights.size()); ++i) {
            graph->update_edge_weight(modified_from[i], modified_to[i], current_weights[static_cast<std::size_t>(i)]);
        }
        delete[] city_ids;
        return ret;
    }

    for (int i = 0; i < modify_count; ++i) {
        ret = graph->update_edge_weight(modified_from[i], modified_to[i], current_weights[static_cast<std::size_t>(i)]);
        if (ret != SUCCESS) {
            delete[] city_ids;
            return ret;
        }
    }

    ret = run_dijkstra_core(graph, start_city, city_ids, city_count, after_dist, after_prev);
    if (ret == SUCCESS && modify_count == 0) {
        copy_array(after_dist, before_dist, city_count);
        copy_array(after_prev, before_prev, city_count);
    }
    delete[] city_ids;
    return ret;
}

/**
 * @brief 打印拥堵前后最短路径对比报告，以表格形式展示各城市距离变化和路径是否改变。
 * @param start_city 起点城市ID
 */
void CongestionSimulator::print_comparison_report(int start_city) const
{
    if (graph == nullptr) {
        std::cout << "拥堵模拟器尚未初始化。" << std::endl;
        return;
    }

    int ret = const_cast<CongestionSimulator *>(this)->run_comparison(start_city);
    if (ret != SUCCESS) {
        std::cout << "无法生成拥堵对比报告，错误码: " << ret << std::endl;
        return;
    }

    int *city_ids = nullptr;
    int city_count = 0;
    ret = graph->get_all_vertex_ids(&city_ids, &city_count);
    if (ret != SUCCESS || city_count <= 0) {
        delete[] city_ids;
        std::cout << "无法读取城市列表。" << std::endl;
        return;
    }

    std::cout << "=== 拥堵前后最短路径对比报告 ===" << std::endl;
    std::cout << "起点城市: " << start_city << std::endl;
    if (modify_count == 0) {
        std::cout << "当前没有拥堵修改记录。" << std::endl;
    } else {
        std::cout << "当前拥堵道路数: " << modify_count << std::endl;
    }

    std::cout << "------------------------------------------------------------" << std::endl;
    std::cout << "城市\t拥堵前距离\t拥堵后距离\t路径变化" << std::endl;

    for (int i = 0; i < city_count; ++i) {
        City_t city{};
        if (graph->get_vertex(city_ids[i], &city) != SUCCESS) {
            continue;
        }

        bool path_changed = before_prev != nullptr && after_prev != nullptr && before_prev[i] != after_prev[i];
        std::cout << city.id << "(" << city.name << ")\t";
        if (before_dist != nullptr && before_dist[i] != INF_WEIGHT) {
            std::cout << before_dist[i];
        } else {
            std::cout << "INF";
        }

        std::cout << "\t\t";
        if (after_dist != nullptr && after_dist[i] != INF_WEIGHT) {
            std::cout << after_dist[i];
        } else {
            std::cout << "INF";
        }

        std::cout << "\t\t" << (path_changed ? "是" : "否") << std::endl;
    }

    std::cout << "------------------------------------------------------------" << std::endl;
    delete[] city_ids;
}