/*
 * 模块名称  : 数据结构--并查集
 * 编写人员  : 组员B（算法/核心功能负责人）
 * 功能描述  : UnionFind 成员函数实现，用于 Kruskal 最小生成树判环。
 */

#include "union_find.h"
#include "../common/defines.h"

/*初始化并查集*/
UnionFind::UnionFind(int size)
    : parent(nullptr),
      rank(nullptr),
      size(size > 0 ? size : 0) {
    if (this->size <= 0) {
        return;
    }

    parent = new int[this->size];
    rank = new int[this->size];

    for (int i = 0; i < this->size; ++i) {
        parent[i] = i;//每个元素的父节点是自己
        rank[i] = 1;//每个元素的秩是1
    }
}

/*析构函数*/
UnionFind::~UnionFind() {
    safe_delete_array(parent);
    safe_delete_array(rank);
}

int UnionFind::find_root(int x) {
    if (x < 0 || x >= size || parent == nullptr) {
        return -1;
    }//越界或未初始化

    if (parent[x] != x) {
        parent[x] = find_root(parent[x]);
    }//路径压缩

    return parent[x];
}

/*按秩合并*/
void UnionFind::union_sets(int x, int y) {
    int root_x = find_root(x);
    int root_y = find_root(y);

    if (root_x == -1 || root_y == -1 || root_x == root_y) {
        return;
    }

    if (rank[root_x] < rank[root_y]) {
        parent[root_x] = root_y;
    } else if (rank[root_x] > rank[root_y]) {
        parent[root_y] = root_x;
    } else {
        parent[root_y] = root_x;
        ++rank[root_x];
    }
}

/*判断两个元素是否连通*/
bool UnionFind::is_connected(int x, int y) {
    int root_x = find_root(x);
    int root_y = find_root(y);

    return root_x != -1 && root_x == root_y;
}

/*计算连通分量的个数*/
int UnionFind::count_sets() const {
    if (parent == nullptr) {
        return 0;
    }

    int count = 0;
    for (int i = 0; i < size; ++i) {
        if (parent[i] == i) {
            ++count;
        }
    }

    return count;
}
