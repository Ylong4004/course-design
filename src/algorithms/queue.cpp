/*
 * 模块名称  : 数据结构——循环队列
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : Queue成员函数的编码
 */

#include "queue.h"

/*构造 析构*/
Queue::Queue(int cap)
    : data(nullptr), front(0), rear(0), capacity(cap), size(0)
{
    data = new int[cap]();
}
Queue::~Queue()
{
    delete[] data;
    data = nullptr;
}

/*基本操作*/
void Queue::push(int value)
{
    // TODO: 判满 → data[rear] = value → rear = (rear+1) % capacity → size++
}

int Queue::pop()
{
    // TODO: 判空 → int val = data[front] → front = (front+1) % capacity → size-- → return val
    return 0;
}

int Queue::top() const
{
    // TODO: 判空 → return data[front]
    return 0;
}

/*状态查询*/
bool Queue::empty() const { return size == 0; }
bool Queue::full()  const { return size == capacity; }
int  Queue::get_size() const { return size; }
int  Queue::get_capacity() const { return capacity; }

void Queue::clear()
{
    front = 0;
    rear = 0;
    size = 0;
}
