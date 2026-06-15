/*
 * 模块名称  : 算法——拓扑排序
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 拓扑排序算法声明（Kahn 算法 / DFS 后序）。
 *             用于有向图的路网通行序列检测与环路判定。
 */

#ifndef TOPOLOGICAL_H
#define TOPOLOGICAL_H

#include "../graph/graph_base.h"

/**
 * @brief  拓扑排序（Kahn 算法 + DFS 两种实现）
 * @param  graph       图存储对象指针（须为有向图）
 * @param  out_sequence  输出：拓扑序列（动态数组，调用方释放）
 * @param  out_length    输出：序列长度
 * @param  has_cycle    输出：是否存在环路（true = 有环，无法完全拓扑排序）
 * @return 成功返回 SUCCESS
 *
 * @note   使用 Kahn 算法（入度表 + 队列）
 * @note   时间复杂度 O(V+E)，空间复杂度 O(V)
 */
int run_topological_sort(const GraphBase *graph,
                       int **out_sequence,
                       int *out_length,
                       bool *has_cycle);

/**
 * @brief  检测图中是否存在环路（为拓扑排序的快捷接口）
 * @return 有环返回 true，无环返回 false
 */
bool has_cycle(const GraphBase *graph);

/**
 * @brief  格式化输出拓扑排序结果
 */
void print_topo_result(const GraphBase *graph,
                     const int *sequence,
                     int length,
                     bool has_cycle);

#endif /* TOPOLOGICAL_H */
