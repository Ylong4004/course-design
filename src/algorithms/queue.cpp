/*
 * 模块名称  : 数据结构--循环队列
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : Queue 成员函数实现，用于 BFS 和 Kahn 拓扑排序。
 */

#include "queue.h"
#include "../common/defines.h"

/**
 * @brief 构造函数，初始化循环队列
 * @param cap 队列容量
 */
Queue::Queue(int cap)
    : data(nullptr),
      front(0),
      rear(0),
      capacity(cap > 0 ? cap : 0),
      size(0) {
    if (capacity > 0) {
        safe_new_array(data, int, capacity);
        for (int i = 0; i < capacity; ++i) data[i] = 0;
    }
}

/**
 * @brief 析构函数，释放队列内存
 */
Queue::~Queue() {
    safe_delete_array(data);
}

/**
 * @brief 入队操作，将元素添加到队尾
 * @param value 要入队的元素值
 */
void Queue::push(int value) {
    if (full()) {
        return;
    }

    data[rear] = value;
    rear = (rear + 1) % capacity;
    ++size;
}

/**
 * @brief 出队操作，移除并返回队首元素
 * @return 队首元素值，队列为空时返回 -1
 */
int Queue::pop() {
    if (empty()) {
        return -1;
    }

    int value = data[front];
    front = (front + 1) % capacity;
    --size;
    return value;
}

/**
 * @brief 查看队首元素但不移除
 * @return 队首元素值，队列为空时返回 -1
 */
int Queue::top() const {
    if (empty()) {
        return -1;
    }

    return data[front];
}

/**
 * @brief 判断队列是否为空
 * @return 队列为空返回 true，否则返回 false
 */
bool Queue::empty() const {
    return size == 0;
}

/**
 * @brief 判断队列是否已满
 * @return 队列已满返回 true，否则返回 false
 */
bool Queue::full() const {
    return capacity <= 0 || size == capacity;
}

/**
 * @brief 获取当前队列元素个数
 * @return 队列中元素的数量
 */
int Queue::get_size() const {
    return size;
}

/**
 * @brief 获取队列容量
 * @return 队列的最大容量
 */
int Queue::get_capacity() const {
    return capacity;
}

/**
 * @brief 清空队列，重置队首队尾指针
 */
void Queue::clear() {
    front = 0;
    rear = 0;
    size = 0;
}
