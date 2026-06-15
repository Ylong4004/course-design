#include "Queue.h"

/* 构造函数 */
Queue::Queue(int capacity) {
    // 分配数组空间，容量至少为 1（避免除零或空队列）
    if (capacity <= 0) capacity = 1;
    m_capacity = capacity;
    m_p_data = new int[m_capacity];
    m_front = 0;
    m_rear = 0;
    m_size = 0;
}

/* 析构函数 */
Queue::~Queue() {
    delete[] m_p_data;
    m_p_data = nullptr;
}

/* 入队操作 */
int Queue::enqueue(int value) {
    if (is_full()) {
        return -1;   // 队列已满，入队失败
    }
    m_p_data[m_rear] = value;
    m_rear = (m_rear + 1) % m_capacity;
    ++m_size;
    return 0;
}

/* 出队操作 */
int Queue::dequeue(int* p_out_value) {
    if (is_empty()) {
        return -1;   // 队列为空，出队失败
    }
    if (p_out_value != nullptr) {
        *p_out_value = m_p_data[m_front];
    }
    m_front = (m_front + 1) % m_capacity;
    --m_size;
    return 0;
}

/* 查看队首元素（不删除） */
int Queue::peek(int* p_out_value) const {
    if (is_empty()) {
        return -1;
    }
    if (p_out_value != nullptr) {
        *p_out_value = m_p_data[m_front];
    }
    return 0;
}

/* 判空 */
bool Queue::is_empty() const {
    return m_size == 0;
}

/* 判满 */
bool Queue::is_full() const {
    return m_size == m_capacity;
}

/* 获取当前元素个数 */
int Queue::get_size() const {
    return m_size;
}

/* 获取队列容量 */
int Queue::get_capacity() const {
    return m_capacity;
}

/* 清空队列 */
void Queue::clear() {
    m_front = 0;
    m_rear = 0;
    m_size = 0;
}