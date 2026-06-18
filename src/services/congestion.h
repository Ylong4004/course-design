/*
 * 模块名称  : 服务——拥堵模拟
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 交通拥堵仿真模拟——修改道路权重，对比拥堵前后路径变化。
 */

#ifndef CONGESTION_H
#define CONGESTION_H

#include "../graph/graph_base.h"

/**
 * @class CongestionSimulator
 * @brief 拥堵模拟器
 *
 * 记录拥堵前后的道路权值变化，支持：
 * - 单条道路权值修改（模拟拥堵）
 * - 批量恢复
 * - 拥堵前后最短路径对比
 */
class CongestionSimulator
{
private:
    GraphBase *graph; /* 当前操作的图存储 */

    /* 拥堵记录（用于恢复） */
    int *modified_from;   /* 被修改道路的起点列表 */
    int *modified_to;     /* 被修改道路的终点列表 */
    int *original_weight; /* 被修改道路的原始权值 */
    int modify_count;     /* 被修改道路数量 */
    int max_modify;       /* 最大可记录修改数 */

    /* 对比数据（拥堵前后的路径结果缓存） */
    int *before_dist;       /* 拥堵前各城市距离 */
    int *before_prev;       /* 拥堵前前驱数组 */
    int *after_dist;        /* 拥堵后各城市距离 */
    int *after_prev;        /* 拥堵后前驱数组 */
    int analysis_city_count; /* 分析的城市数量 */

public:
    CongestionSimulator(GraphBase *graph, int max_modify);
    ~CongestionSimulator();

    /* ========== 拥堵操作 ========== */

    /**
     * @brief  设置指定道路为拥堵状态（增大权值）
     * @param  from        起点
     * @param  to          终点
     * @param  congestion_weight  拥堵后的新权值
     * @note   自动备份原始权值用于恢复
     */
    int set_congestion(int from, int to, int congestion_weight);

    /** @brief 恢复全部拥堵道路到原始权值 */
    int restore_all();

    /** @brief 查看当前被修改的道路列表 */
    void list_modified_roads() const;

    /* ========== 对比分析 ========== */

    /**
     * @brief  执行拥堵前后对比分析
     * @param  start_city  分析起点城市
     * @note   分别用拥堵前后的路网运行 Dijkstra，缓存结果
     */
    int run_comparison(int start_city);

    /** @brief 打印拥堵前后最短路径对比报告 */
    void print_comparison_report(int start_city) const;
};

#endif /* CONGESTION_H */
