/*
 * 模块名称  : 数据结构——最小堆/优先队列
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 基于二叉堆的最小优先队列实现，支持插入、删除最小元素。
 *            用于 Dijkstra 和 Prim 算法的 O((V+E)logV) 优化版本。
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

/**
 * @class PriorityQueue
 * @brief 最小堆优先队列（二叉堆实现）
 *
 * 堆中每个元素是一个 (vertex, priority) 键值对，
 * 优先级最小的元素始终在堆顶。
 *
 * 使用方式（STL 风格）：
 *   pq.push(v, dist);
 *   while (!pq.empty()) { int v = pq.pop(); ... }
 *
 * 时间复杂度：
 *   push         O(log V)
 *   pop          O(log V)
 *   decrease_key O(log V)
 *   top          O(1)
 */
class PriorityQueue {
private:
    struct HeapNode {
        int vertex;    /* 顶点编号 */
        int priority;  /* 优先级（距离/权值） */
    };

    HeapNode* heap;    /* 堆数组（下标从 0 开始） */
    int*      pos;     /* pos[v] = 顶点 v 在堆中的位置，-1 表示不在堆中 */
    int       capacity;  /* 最大容量 */
    int       size;      /* 当前堆大小 */

public:
    /* ========== 构造/析构 ========== */

    /**
     * @brief 初始化优先队列
     * @param cap  最大容量（顶点数量）
     */
    PriorityQueue(int cap);
    ~PriorityQueue();

    /* ========== 核心操作 ========== */

    /** @brief 插入一个顶点及其优先级 */
    void push(int vertex, int priority);

    /** @brief 取出并返回优先级最小的顶点编号，空堆时行为未定义 */
    int pop();

    /** @brief 查看堆顶顶点编号（不出队），空堆时行为未定义 */
    int top() const;

    /**
     * @brief  降低指定顶点的优先级（Dijkstra 松弛操作专用）
     * @param  vertex       目标顶点
     * @param  new_priority 新的更小优先级
     * @note   新优先级必须 <= 旧优先级，否则行为未定义
     */
    void decrease_key(int vertex, int new_priority);

    /* ========== 状态查询 ========== */

    bool empty() const;
    bool contains(int vertex) const;
    int  get_size() const;
    int  get_capacity() const;

    /** @brief 清空堆 */
    void clear();

private:
    /* ========== 堆内部操作 ========== */

    /** @brief 上浮（插入后调整） */
    void sift_up(int index);

    /** @brief 下沉（删除后调整） */
    void sift_down(int index);

    /** @brief 交换堆中两个位置的元素 */
    void swap_nodes(int i, int j);
};

#endif /* PRIORITY_QUEUE_H */
