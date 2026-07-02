/*
 * 模块名称  : 算法——最短路径（Dijkstra & Floyd）
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : Dijkstra 单源最短路径 + Floyd 多源最短路径算法声明。
 *             通过 GraphBase 指针操作，与具体存储结构解耦。
 */

#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "../graph/graph_base.h"

/* ========================= Dijkstra ========================== */

/**
 * @brief  Dijkstra 单源最短路径
 * @param  graph     图存储对象指针
 * @param  start_city  起点城市编号
 * @param  out_dist    输出：到各城市的最短距离数组（按 get_all_vertex_ids 顺序存储）
 * @param  out_prev    输出：前驱城市编号数组（按 get_all_vertex_ids 顺序存储）
 * @return 成功返回 SUCCESS
 *
 * @note   适用于非负权图
 * @note   邻接矩阵实现 O(V²)，邻接表 + 优先队列实现 O((V+E)·logV)
 */
int run_dijkstra(const GraphBase *graph,
                int start_city,
                int *out_dist,
                int *out_prev);

/**
 * @brief  根据 Dijkstra 结果回溯具体路径
 * @param  graph      图存储对象指针
 * @param  prev       前驱数组（来自 run_dijkstra）
 * @param  vertex_count 顶点数
 * @param  start_city  起点
 * @param  end_city    终点
 * @param  out_path    输出：路径顶点序列
 * @param  out_len     输出：路径长度
 * @return 成功返回 SUCCESS
 */
int dijkstra_get_path(const GraphBase *graph,
                    const int *prev,
                    int vertex_count,
                    int start_city,
                    int end_city,
                    int **out_path,
                    int *out_len);

/**
 * @brief  格式化输出 Dijkstra 结果（起点→各城市距离和路径）
 */
void print_dijkstra_result(const GraphBase *graph,
                         int start_city,
                         const int *dist,
                         const int *prev);

/* ========================= Floyd ========================== */

/**
 * @brief  Floyd 多源最短路径
 * @param  graph      图存储对象指针
 * @param  out_dist     输出：V×V 距离矩阵（i/j 为 get_all_vertex_ids 顺序下标）
 * @param  out_next     输出：V×V 路径矩阵（值为下一步顶点的内部下标）
 * @param  vertex_count 输出：顶点数量
 * @return 成功返回 SUCCESS
 *
 * @note   时间复杂度 O(V³)，空间复杂度 O(V²)
 * @note   调用方负责释放 out_dist 和 out_next 二维数组
 */
int run_floyd(const GraphBase *graph,
             int ***out_dist,
             int ***out_next,
             int *vertex_count);

/**
 * @brief  根据 Floyd 结果重建 i→j 的最短路径
 * @param  graph      图存储对象指针
 * @param  next       Floyd 路径矩阵
 * @param  vertex_count 顶点数
 * @param  from       起点
 * @param  to         终点
 * @param  out_path    输出：路径序列
 * @param  out_len     输出：路径长度
 */
int floyd_get_path(const GraphBase *graph,
                 int **next,
                 int vertex_count,
                 int from, int to,
                 int **out_path,
                 int *out_len);

/**
 * @brief  格式化输出 Floyd 全路径距离对照表
 */
void print_floyd_table(const GraphBase *graph,
                     int **dist,
                     int **next,
                     int vertex_count);

#endif /* SHORTEST_PATH_H */
