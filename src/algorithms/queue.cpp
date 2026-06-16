/*
 * 模块名称  : 数据结构--循环队列
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : Queue 成员函数实现，用于 BFS 和 Kahn 拓扑排序。
 */

#include "queue.h"
#include "../common/defines.h"

Queue::Queue(int cap)
    : data(nullptr),
      front(0),
      rear(0),
      capacity(cap > 0 ? cap : 0),
      size(0) {
    if (capacity > 0) {
        data = new int[capacity]();
    }
}

Queue::~Queue() {
    safe_delete_array(data);
}

void Queue::push(int value) {
    if (full()) {
        return;
    }

    data[rear] = value;
    rear = (rear + 1) % capacity;
    ++size;
}

int Queue::pop() {
    if (empty()) {
        return -1;
    }

    int value = data[front];
    front = (front + 1) % capacity;
    --size;
    return value;
}

int Queue::top() const {
    if (empty()) {
        return -1;
    }

    return data[front];
}

bool Queue::empty() const {
    return size == 0;
}

bool Queue::full() const {
    return capacity <= 0 || size == capacity;
}

int Queue::get_size() const {
    return size;
}

int Queue::get_capacity() const {
    return capacity;
}

void Queue::clear() {
    front = 0;
    rear = 0;
    size = 0;
}
