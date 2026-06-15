/*
 * 模块名称  : 数据结构——循环队列
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 基于数组的循环队列实现，用于 BFS 和 Kahn 拓扑排序。
 *            先入先出（FIFO），支持判空、判满、入队、出队。
 */

#ifndef QUEUE_H
#define QUEUE_H

/**
 * @class Queue
 * @brief 循环队列（数组实现）
 *
 * 使用固定容量数组 + 头尾双指针实现循环复用空间。
 * 用于 BFS 广度优先遍历、Kahn 拓扑排序中管理待处理顶点。
 */
class Queue {
private:
    int* data;       /* 队列数据数组 */
    int  front;        /* 队首索引 */
    int  rear;         /* 队尾索引 */
    int  capacity;     /* 最大容量 */
    int  size;         /* 当前元素个数 */

public:
    /* ========== 构造/析构 ========== */
    Queue(int capacity);
    ~Queue();

    /* ========== 基本操作 ========== */

    /** @brief 入队（从队尾插入） */
    int enqueue(int value);

    /** @brief 出队（从队首删除） */
    int dequeue(int* out_value);

    /** @brief 查看队首元素（不出队） */
    int peek(int* out_value) const;

    /* ========== 状态查询 ========== */

    bool is_empty() const;
    bool is_full()  const;
    int  get_size()    const;
    int  get_capacity() const;

    /** @brief 清空队列（重置头尾指针） */
    void clear();
};

#endif /* QUEUE_H */
