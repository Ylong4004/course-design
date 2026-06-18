/*
 * 模块名称  : 数据结构--并查集
 * 编写人员  : 组员A（架构负责人）
 * 功能描述  : 并查集（Disjoint Set Union）实现，支持路径压缩与按秩合并。
 *             用于 Kruskal 最小生成树算法中判断两端点是否属于同一集合。
 */

#ifndef UNION_FIND_H
#define UNION_FIND_H

/**
 * @class UnionFind
 * @brief 并查集（森林实现）
 *
 * 使用双亲数组表示集合关系：
 * - parent[i] = i  表示 i 是所在集合的根结点
 * - parent[i] = j  表示 i 的父结点是 j
 * - rank[i]        表示以 i 为根的树高上界，用于按秩合并
 *
 * find_root 使用路径压缩；union_sets 使用按秩合并。
 */
class UnionFind {
private:
    int* parent;    /* 双亲数组 */
    int* rank;      /* 秩数组（树高上界） */
    int  size;      /* 元素总数 */

public:
    UnionFind(int size);
    ~UnionFind();

    int find_root(int x);
    void union_sets(int x, int y);
    bool is_connected(int x, int y);
    int count_sets() const;
};

#endif /* UNION_FIND_H */
