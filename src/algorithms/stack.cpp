/*
 * 模块名称  : 数据结构--顺序栈
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : Stack 成员函数实现，用于 DFS 非递归遍历和路径回溯。
 */

#include "stack.h"
#include "../common/defines.h"

Stack::Stack(int cap)
    : data(nullptr),
      top_index(-1),
      capacity(cap > 0 ? cap : 0) {
    if (capacity > 0) {
        data = new int[capacity]();
    }
}

Stack::~Stack() {
    safe_delete_array(data);
}

void Stack::push(int value) {
    if (full()) {
        return;
    }

    data[++top_index] = value;
}

int Stack::pop() {
    if (empty()) {
        return -1;
    }

    return data[top_index--];
}

int Stack::top() const {
    if (empty()) {
        return -1;
    }

    return data[top_index];
}

bool Stack::empty() const {
    return top_index == -1;
}

bool Stack::full() const {
    return capacity <= 0 || top_index == capacity - 1;
}

int Stack::get_size() const {
    return top_index + 1;
}

int Stack::get_capacity() const {
    return capacity;
}

void Stack::clear() {
    top_index = -1;
}
