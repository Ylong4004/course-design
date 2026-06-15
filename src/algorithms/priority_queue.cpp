/*
 * 模块名称  : 数据结构--最小堆/优先队列
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : PriorityQueue 成员函数实现，用于 Dijkstra 和 Prim。
 */

#include "priority_queue.h"

PriorityQueue::PriorityQueue(int cap)
    : heap(nullptr),
      pos(nullptr),
      capacity(cap > 0 ? cap : 0),
      size(0) {
    if (capacity <= 0) {
        return;
    }

    heap = new HeapNode[capacity];
    pos = new int[capacity];

    for (int i = 0; i < capacity; ++i) {
        pos[i] = -1;
    }
}

PriorityQueue::~PriorityQueue() {
    delete[] heap;
    delete[] pos;
    heap = nullptr;
    pos = nullptr;
}

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

int PriorityQueue::top() const {
    if (empty()) {
        return -1;
    }

    return heap[0].vertex;
}

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

bool PriorityQueue::empty() const {
    return size == 0;
}

bool PriorityQueue::contains(int vertex) const {
    return vertex >= 0 &&
           vertex < capacity &&
           pos != nullptr &&
           pos[vertex] != -1;
}

int PriorityQueue::get_size() const {
    return size;
}

int PriorityQueue::get_capacity() const {
    return capacity;
}

void PriorityQueue::clear() {
    size = 0;

    if (pos == nullptr) {
        return;
    }

    for (int i = 0; i < capacity; ++i) {
        pos[i] = -1;
    }
}

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

void PriorityQueue::swap_nodes(int i, int j) {
    HeapNode temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;

    pos[heap[i].vertex] = i;
    pos[heap[j].vertex] = j;
}
