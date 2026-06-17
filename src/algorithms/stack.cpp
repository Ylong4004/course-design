/*
 * 模块名称  : 数据结构--顺序栈
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : Stack 成员函数实现，用于 DFS 非递归遍历和路径回溯。
 */

#include "stack.h"
#include "../common/defines.h"

/**
 * @brief 构造函数，初始化顺序栈
 * @param cap 栈容量
 */
Stack::Stack(int cap)
    : data(nullptr),
      top_index(-1),
      capacity(cap > 0 ? cap : 0) {
    if (capacity > 0) {
        safe_new_array(data, int, capacity);
        for (int i = 0; i < capacity; ++i) data[i] = 0;
    }
}

/**
 * @brief 析构函数，释放栈内存
 */
Stack::~Stack() {
    safe_delete_array(data);
}

/**
 * @brief 压栈操作，将元素放入栈顶
 * @param value 要压栈的元素值
 */
void Stack::push(int value) {
    if (full()) {
        return;
    }

    data[++top_index] = value;
}

/**
 * @brief 出栈操作，移除并返回栈顶元素
 * @return 栈顶元素值，栈为空时返回 -1
 */
int Stack::pop() {
    if (empty()) {
        return -1;
    }

    return data[top_index--];
}

/**
 * @brief 查看栈顶元素但不移除
 * @return 栈顶元素值，栈为空时返回 -1
 */
int Stack::top() const {
    if (empty()) {
        return -1;
    }

    return data[top_index];
}

/**
 * @brief 判断栈是否为空
 * @return 栈为空返回 true，否则返回 false
 */
bool Stack::empty() const {
    return top_index == -1;
}

/**
 * @brief 判断栈是否已满
 * @return 栈已满返回 true，否则返回 false
 */
bool Stack::full() const {
    return capacity <= 0 || top_index == capacity - 1;
}

/**
 * @brief 获取当前栈中元素个数
 * @return 栈中元素的数量
 */
int Stack::get_size() const {
    return top_index + 1;
}

/**
 * @brief 获取栈容量
 * @return 栈的最大容量
 */
int Stack::get_capacity() const {
    return capacity;
}

/**
 * @brief 清空栈，重置栈顶指针
 */
void Stack::clear() {
    top_index = -1;
}
