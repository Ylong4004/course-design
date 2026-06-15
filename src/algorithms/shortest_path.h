/*
 * 模块名称  : 算法——最短路径（Dijkstra & Floyd）
 * 编写人    : 组员B（算法&核心功能负责人）
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
 * @param  out_dist    输出：到各城市的最短距离数组（长度 = 顶点数）
 * @param  out_prev    输出：前驱顶点数组（用于路径回溯，长度 = 顶点数）
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
 * @param  prev       前驱数组（来自 run_dijkstra）
 * @param  vertex_count 顶点数
 * @param  start_city  起点
 * @param  end_city    终点
 * @param  out_path    输出：路径顶点序列
 * @param  out_len     输出：路径长度
 * @return 成功返回 SUCCESS
 */
int dijkstra_get_path(const int *prev,
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
 * @param  out_dist     输出：V×V 距离矩阵（out_dist[i][j] = i→j 最短距离）
 * @param  out_next     输出：V×V 路径矩阵（out_next[i][j] = i→j 下一步顶点）
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
 * @param  next       Floyd 路径矩阵
 * @param  vertex_count 顶点数
 * @param  from       起点
 * @param  to         终点
 * @param  out_path    输出：路径序列
 * @param  out_len     输出：路径长度
 */
int floyd_get_path(int **next,
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
