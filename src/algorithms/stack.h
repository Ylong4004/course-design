/*
 * 模块名称  : 数据结构——顺序栈
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : 基于数组的顺序栈实现，用于 DFS 非递归遍历。
 *            后入先出（LIFO），支持判空、判满、压栈、弹栈。
 */

#ifndef STACK_H
#define STACK_H

/**
 * @class Stack
 * @brief 顺序栈（数组实现）
 *
 * 使用固定容量数组 + 栈顶指针。
 * 用于 DFS 深度优先遍历的非递归实现，以及路径回溯。
 *
 * 使用方式（STL 风格，调用者自行判空/判满）：
 *   while (!s.empty()) { int v = s.pop(); ... }
 */
class Stack {
private:
    int* data;          /* 栈数据数组 */
    int  top_index;     /* 栈顶索引（-1 表示空栈） */
    int  capacity;      /* 最大容量 */

public:
    /* ========== 构造/析构 ========== */
    Stack(int cap);
    ~Stack();

    /* ========== 基本操作 ========== */

    /** @brief 压栈（将元素放入栈顶），满栈时行为未定义 */
    void push(int value);

    /** @brief 弹栈（取出栈顶元素并移除），空栈时行为未定义 */
    int pop();

    /** @brief 查看栈顶元素（不弹出），空栈时行为未定义 */
    int top() const;

    /* ========== 状态查询 ========== */

    bool empty() const;
    bool full()  const;
    int  get_size() const;
    int  get_capacity() const;

    /** @brief 清空栈（重置栈顶指针） */
    void clear();
};

#endif /* STACK_H */
