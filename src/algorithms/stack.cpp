/*
 * 模块名称  : 数据结构——顺序栈
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : Stack成员函数的编码
 */

#include "stack.h"

/*构造 析构*/
Stack::Stack(int cap)
    : data(nullptr), top_index(-1), capacity(cap)
{
    data = new int[cap]();
}
Stack::~Stack()
{
    delete[] data;
    data = nullptr;
}

/*基本操作*/
void Stack::push(int value)
{
    // TODO: 判满 → top_index++ → data[top_index] = value
}

int Stack::pop()
{
    // TODO: 判空 → int val = data[top_index] → top_index-- → return val
    return 0;
}

int Stack::top() const
{
    // TODO: 判空 → return data[top_index]
    return 0;
}

/*状态查询*/
bool Stack::empty() const { return top_index == -1; }
bool Stack::full()  const { return top_index == capacity - 1; }
int  Stack::get_size() const { return top_index + 1; }
int  Stack::get_capacity() const { return capacity; }

void Stack::clear()
{
    top_index = -1;
}
