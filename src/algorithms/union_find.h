/*
 * 模块名称  : 数据结构——并查集
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 并查集（Disjoint Set Union）实现，支持路径压缩与按秩合并。
 *            用于 Kruskal 最小生成树算法中判断边的两端是否属于同一集合。
 */

#ifndef UNION_FIND_H
#define UNION_FIND_H

/**
 * @class UnionFind
 * @brief 并查集（森林实现）
 *
 * 使用双亲表示法的树形结构存储集合关系：
 * - parent[i] = j  表示 i 的父结点是 j
 * - parent[i] = -1 表示 i 是根结点
 * - rank[i]        表示以 i 为根的树的高度（用于按秩合并）
 *
 * 时间复杂度：查找 O(α(V)) ≈ O(1)，合并 O(α(V)) ≈ O(1)
 *            其中 α 是反阿克曼函数，实际可视为常数
 */
class UnionFind {
private:
    int* parent;    /* 双亲数组 */
    int* rank;      /* 秩数组（树高上界） */
    int  size;        /* 元素总数 */

public:
    /* ========== 构造/析构 ========== */

    /**
     * @brief 初始化并查集，每个元素自成一个集合
     * @param size  元素总数（顶点数量）
     */
    UnionFind(int size);
    ~UnionFind();

    /* ========== 核心操作 ========== */

    /**
     * @brief  查找元素 x 所属集合的根结点（带路径压缩）
     * @param  x  元素编号
     * @return 根结点编号
     */
    int find_root(int x);

    /**
     * @brief  合并 x 和 y 所在的两个集合（按秩合并）
     * @param  x  第一个元素
     * @param  y  第二个元素
     */
    void union_sets(int x, int y);

    /**
     * @brief  判断两个元素是否属于同一集合
     * @return 同一集合返回 true
     */
    bool is_connected(int x, int y);

    /* ========== 状态查询 ========== */

    /** @brief 统计当前集合数量 */
    int count_sets() const;
};

#endif /* UNION_FIND_H */
