/*
 * 模块名称  : 算法——图遍历（DFS & BFS）
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 深度优先遍历(DFS)和广度优先遍历(BFS)算法声明。
 *             通过 GraphBase 指针操作，与具体存储结构解耦。
 */

#ifndef TRAVERSAL_H
#define TRAVERSAL_H

#include "../graph/graph_base.h"

/**
 * @brief  深度优先遍历（DFS）
 * @param  graph      图存储对象指针
 * @param  start_city   起始城市编号
 * @param  out_sequence 输出：遍历序列（动态数组，调用方释放）
 * @param  out_length    输出：序列长度
 * @return 成功返回 SUCCESS
 *
 * @note   递归实现（或显式栈实现）
 * @note   时间复杂度 O(V+E)，空间复杂度 O(V)
 */
int traverse_dfs(const GraphBase *graph,
                int start_city,
                int **out_sequence,
                int *out_length);

/**
 * @brief  广度优先遍历（BFS）
 * @param  graph      图存储对象指针
 * @param  start_city   起始城市编号
 * @param  out_sequence 输出：遍历序列（动态数组，调用方释放）
 * @param  out_length    输出：序列长度
 * @return 成功返回 SUCCESS
 *
 * @note   使用队列辅助实现
 * @note   时间复杂度 O(V+E)，空间复杂度 O(V)
 */
int traverse_bfs(const GraphBase *graph,
                int start_city,
                int **out_sequence,
                int *out_length);

/**
 * @brief  打印遍历序列（格式化输出）
 * @param  graph    图存储对象指针
 * @param  sequence  遍历序列数组
 * @param  length    序列长度
 * @param  algo_name  算法名称（如 "DFS"）
 */
void print_traversal_sequence(const GraphBase *graph,
                            const int *sequence,
                            int length,
                            const char *algo_name);

#endif /* TRAVERSAL_H */
