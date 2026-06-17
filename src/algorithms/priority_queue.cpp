/*
 * 模块名称  : 数据结构--最小堆/优先队列
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : PriorityQueue 成员函数实现，用于 Dijkstra 和 Prim。
 */

#include "priority_queue.h"
#include "../common/defines.h"

/**
 * @brief 构造函数，初始化最小堆优先队列
 * @param cap 优先队列容量（最大顶点数）
 */
PriorityQueue::PriorityQueue(int cap)
    : heap(nullptr),
      pos(nullptr),
      capacity(cap > 0 ? cap : 0),
      size(0) {
    if (capacity <= 0) {
        return;
    }

    safe_new_array(heap, HeapNode, capacity);
    safe_new_array(pos, int, capacity);

    for (int i = 0; i < capacity; ++i) {
        pos[i] = -1;
    }
}

/**
 * @brief 析构函数，释放堆和位置数组内存
 */
PriorityQueue::~PriorityQueue() {
    safe_delete_array(heap);
    safe_delete_array(pos);
}

/**
 * @brief 将顶点及其优先级入队，若顶点已存在且新优先级更低则更新
 * @param vertex 顶点编号
 * @param priority 优先级（权值）
 */
void PriorityQueue::push(int vertex, int priority) {
    if (vertex < 0 || vertex >= capacity || heap == nullptr || pos == nullptr) {
        return;
    }

    if (contains(vertex)) {
        if (priority < heap[pos[vertex]].priority) {
            decrease_key(vertex, priority);
        }
        return;
    }

    if (size >= capacity) {
        return;
    }

    heap[size].vertex = vertex;
    heap[size].priority = priority;
    pos[vertex] = size;
    sift_up(size);
    ++size;
}

/**
 * @brief 弹出优先级最小（堆顶）的顶点
 * @return 优先级最小的顶点编号，队列为空时返回 -1
 */
int PriorityQueue::pop() {
    if (empty()) {
        return -1;
    }

    int min_vertex = heap[0].vertex;
    pos[min_vertex] = -1;
    --size;

    if (size > 0) {
        heap[0] = heap[size];
        pos[heap[0].vertex] = 0;
        sift_down(0);
    }

    return min_vertex;
}

/**
 * @brief 查看堆顶顶点但不弹出
 * @return 优先级最小的顶点编号，队列为空时返回 -1
 */
int PriorityQueue::top() const {
    if (empty()) {
        return -1;
    }

    return heap[0].vertex;
}

/**
 * @brief 降低指定顶点的优先级并上浮调整堆
 * @param vertex 顶点编号
 * @param new_priority 新的优先级（必须比原优先级小）
 */
void PriorityQueue::decrease_key(int vertex, int new_priority) {
    if (!contains(vertex)) {
        return;
    }

    int index = pos[vertex];
    if (new_priority > heap[index].priority) {
        return;
    }

    heap[index].priority = new_priority;
    sift_up(index);
}

/**
 * @brief 判断优先队列是否为空
 * @return 为空返回 true，否则返回 false
 */
bool PriorityQueue::empty() const {
    return size == 0;
}

/**
 * @brief 检查顶点是否在优先队列中
 * @param vertex 顶点编号
 * @return 存在返回 true，否则返回 false
 */
bool PriorityQueue::contains(int vertex) const {
    return vertex >= 0 &&
           vertex < capacity &&
           pos != nullptr &&
           pos[vertex] != -1;
}

/**
 * @brief 获取优先队列当前元素个数
 * @return 元素数量
 */
int PriorityQueue::get_size() const {
    return size;
}

/**
 * @brief 获取优先队列容量
 * @return 最大容量
 */
int PriorityQueue::get_capacity() const {
    return capacity;
}

/**
 * @brief 清空优先队列，重置所有顶点位置
 */
void PriorityQueue::clear() {
    size = 0;

    if (pos == nullptr) {
        return;
    }

    for (int i = 0; i < capacity; ++i) {
        pos[i] = -1;
    }
}

/**
 * @brief 堆上浮操作，将指定位置的节点向上调整以维持最小堆性质
 * @param index 要上浮的节点下标
 */
void PriorityQueue::sift_up(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (heap[parent].priority <= heap[index].priority) {
            break;
        }

        swap_nodes(parent, index);
        index = parent;
    }
}

/**
 * @brief 堆下沉操作，将指定位置的节点向下调整以维持最小堆性质
 * @param index 要下沉的节点下标
 */
void PriorityQueue::sift_down(int index) {
    while (true) {
        int left = index * 2 + 1;
        int right = index * 2 + 2;
        int smallest = index;

        if (left < size &&
            heap[left].priority < heap[smallest].priority) {
            smallest = left;
        }

        if (right < size &&
            heap[right].priority < heap[smallest].priority) {
            smallest = right;
        }

        if (smallest == index) {
            break;
        }

        swap_nodes(index, smallest);
        index = smallest;
    }
}

/**
 * @brief 交换堆中两个节点的位置，同步更新位置映射数组
 * @param i 第一个节点下标
 * @param j 第二个节点下标
 */
void PriorityQueue::swap_nodes(int i, int j) {
    HeapNode temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;

    pos[heap[i].vertex] = i;
    pos[heap[j].vertex] = j;
}
