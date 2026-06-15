/*
 * 模块名称  : 算法——最小生成树（Prim & Kruskal）
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : Prim 算法与 Kruskal 算法声明。
 *             通过 GraphBase 指针操作，与具体存储结构解耦。
 */

#ifndef SPANNING_TREE_H
#define SPANNING_TREE_H

#include "../graph/graph_base.h"

/**
 * @brief  Prim 算法——最小生成树
 * @param  graph     图存储对象指针
 * @param  out_mst     输出：最小生成树结果（含边集、总造价）
 * @return 成功返回 SUCCESS，图不连通返回 ERR_DISCONNECTED
 *
 * @note   适用于稠密图
 * @note   邻接矩阵实现 O(V²)，邻接表 + 优先队列 O((V+E)·logV)
 */
int build_mst_prim(const GraphBase *graph,
                  MSTResult_t *out_mst);

/**
 * @brief  Kruskal 算法——最小生成树
 * @param  graph     图存储对象指针
 * @param  out_mst     输出：最小生成树结果（含边集、总造价）
 * @return 成功返回 SUCCESS，图不连通返回 ERR_DISCONNECTED
 *
 * @note   适用于稀疏图
 * @note   时间复杂度 O(E·logE)，需要并查集辅助
 * @note   内部使用并查集判断环路
 */
int build_mst_kruskal(const GraphBase *graph,
                     MSTResult_t *out_mst);

/**
 * @brief  格式化输出最小生成树结果
 * @param  algo_name  算法名称（"Prim" / "Kruskal"）
 * @param  mst      生成树结果
 */
void print_mst_result(const char *algo_name,
                    const MSTResult_t *mst);

/**
 * @brief  释放 MST 结果中动态分配的内存
 */
void free_mst_result(MSTResult_t *mst);

/**
 * @brief  并查集——查找根结点（带路径压缩）
 */
int union_find_find(int *parent, int x);

/**
 * @brief  并查集——合并两个集合（按秩合并）
 */
void union_find_union(int *parent, int *rank, int x, int y);

#endif /* SPANNING_TREE_H */
