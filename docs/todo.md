# 开发任务清单

> 当前阶段：框架搭建完成，待编码实现
>
> 交互方式：数字菜单（暂定，后续可改造为命令行风格）

---

## 一、common/ 公共基础层

| # | 文件          | 任务                                        | 负责  |
| - | ------------- | ------------------------------------------- | ----- |
| 1 | `types.h`   | ✅ 已完成（类型定义、结构体、枚举、错误码） | 组员A |
| 2 | `defines.h` | ✅ 已完成（宏定义、调试开关、工具宏）       | 组员A |

---

## 二、graph/ 图存储层

### 2.1 抽象基类

| # | 文件             | 任务                          | 负责  |
| - | ---------------- | ----------------------------- | ----- |
| 3 | `graph_base.h` | ✅ 已完成（26个纯虚接口定义） | 组员A |

### 2.2 邻接矩阵实现

| # | 文件               | 任务                                     | 负责  |
| - | ------------------ | ---------------------------------------- | ----- |
| 4 | `adj_matrix.h`   | ✅ 类声明完成                            | 组员B |
| 5 | `adj_matrix.cpp` | 🔲**待编码** — 实现全部 20 个方法 | 组员B |

具体方法清单：

```
构造/析构:
  - AdjMatrix(max_vertices, graph_type)   // 分配矩阵内存
  - ~AdjMatrix()                          // 释放矩阵内存

存储标识:
  - get_storage_type() → STORAGE_MATRIX
  - get_storage_name() → "邻接矩阵"

图属性:
  - get_vertex_count(), get_edge_count()
  - get_graph_type(), get_max_vertex_count()

顶点操作:
  - add_vertex(city)     // 查重 → 找空位 → 存入 → count++
  - remove_vertex(id)    // 找索引 → 清行列 → 标无效 → count--
  - has_vertex(id)       // find_index != -1
  - get_vertex(id, out)  // find_index → 复制信息

边操作:
  - add_edge(f, t, w)    // 校验 → 查重 → m_p_matrix[i][j] = w → 无向图对称赋值
  - remove_edge(f, t)    // m_p_matrix[i][j] = INF_WEIGHT
  - update_edge_weight() // 检查边存在 → 更新权值
  - get_edge_weight()    // 读矩阵值
  - has_edge()           // 矩阵值 != INF_WEIGHT

遍历支持:
  - get_neighbors(id, out, count)  // 遍历矩阵对应行
  - get_all_vertex_ids(out, count) // 收集有效顶点

性能统计:
  - get_performance_stats()  // 计算内存 = V*V*4 + 顶点数组
  - reset_perf_counters()    // 计数器清零

调试:
  - print_graph()   // 格式化打印矩阵
```

### 2.3 邻接表实现

| # | 文件             | 任务                                     | 负责  |
| - | ---------------- | ---------------------------------------- | ----- |
| 6 | `adj_list.h`   | ✅ 类声明完成                            | 组员B |
| 7 | `adj_list.cpp` | 🔲**待编码** — 实现全部 20 个方法 | 组员B |

具体方法清单：

```
构造/析构:
  - AdjList(max_vertices, graph_type)  // 分配头结点数组
  - ~AdjList()                         // 释放全部边链表 + 头数组

存储标识:
  - get_storage_type() → STORAGE_LIST
  - get_storage_name() → "邻接表"

顶点操作:
  - add_vertex(city)   // 查重 → 找空位 → 存入头结点 → count++
  - remove_vertex(id)  // 删该顶点出边链表 + 遍历其他顶点删除入边

边操作:
  - add_edge(f, t, w)  // 头插法插入边结点 → 无向图双向插入
  - remove_edge(f, t)  // 链表定位删除结点

遍历支持:
  - get_neighbors()      // 遍历 f 的边链表
  - get_all_vertex_ids() // 遍历头数组

私有函数:
  - find_index(city_id)          // 遍历头数组按 id 匹配
  - find_edge_node(from, to)     // 链表中查找
  - free_edge_list(index)        // 释放整条边链表
  - create_edge_node(dest, w)    // new 新边结点
```

---

## 三、algorithms/ 基础数据结构层

| #  | 文件                   | 任务                                               | 负责  |
| -- | ---------------------- | -------------------------------------------------- | ----- |
| 8  | `queue.h`            | ✅ 接口完成                                        | 组员B |
|    | `queue.cpp`          | 🔲**待编码** — 循环队列入队、出队、判空判满 | 组员B |
| 9  | `stack.h`            | ✅ 接口完成                                        | 组员B |
|    | `stack.cpp`          | 🔲**待编码** — 顺序栈压栈、弹栈、判空判满   | 组员B |
| 10 | `union_find.h`       | ✅ 接口完成                                        | 组员B |
|    | `union_find.cpp`     | 🔲**待编码** — 路径压缩查找 + 按秩合并      | 组员B |
| 11 | `priority_queue.h`   | ✅ 接口完成                                        | 组员B |
|    | `priority_queue.cpp` | 🔲**待编码** — 二叉堆上浮下沉 + DecreaseKey | 组员B |

---

## 四、algorithms/ 图算法层

### 4.1 图遍历

| #  | 文件              | 任务               | 负责  |
| -- | ----------------- | ------------------ | ----- |
| 12 | `traversal.h`   | ✅ 接口完成        | 组员B |
|    | `traversal.cpp` | 🔲**待编码** | 组员B |

```
traverse_dfs(p_graph, start, out_seq, out_len)
  - 递归版：visited 数组标记 → 递归访问邻接点
  - 非递归版（加分）：用 Stack 显式模拟递归栈

traverse_bfs(p_graph, start, out_seq, out_len)
  - 用 Queue，起点入队 → 出队访问 → 邻接入队
  - 支持非连通图（遍历未访问顶点）

print_traversal_sequence(p_graph, seq, len, algo_name)
  - 将顶点编号转为城市名称，格式化输出序列
```

### 4.2 最短路径

| #  | 文件                  | 任务               | 负责  |
| -- | --------------------- | ------------------ | ----- |
| 13 | `shortest_path.h`   | ✅ 接口完成        | 组员B |
|    | `shortest_path.cpp` | 🔲**待编码** | 组员B |

```
run_dijkstra(p_graph, start, out_dist, out_prev)
  - 基本版：O(V²) 数组实现
  - 优化版（加分）：用 PriorityQueue 实现 O((V+E)logV)

dijkstra_get_path(prev, n, start, end, out_path, out_len)
  - 从 dist/prev 回溯具体路径序列

run_floyd(p_graph, out_dist, out_next, n)
  - 三重循环 O(V³)，dist[i][j] vs dist[i][k]+dist[k][j]

floyd_get_path(next, n, from, to, out_path, out_len)
  - next[i][j] 回溯路径

print_dijkstra_result() / print_floyd_table()
  - 格式化输出结果表格
```

### 4.3 最小生成树

| #  | 文件                  | 任务               | 负责  |
| -- | --------------------- | ------------------ | ----- |
| 14 | `spanning_tree.h`   | ✅ 接口完成        | 组员B |
|    | `spanning_tree.cpp` | 🔲**待编码** | 组员B |

```
build_mst_prim(p_graph, out_mst)
  - key 数组 + parent 数组 + inMST 标记
  - O(V²) 基本版 或 O((V+E)logV) 优先队列优化版

build_mst_kruskal(p_graph, out_mst)
  - 收集边 → 排序 → UnionFind 判环
  - O(E log E)

print_mst_result(algo_name, p_mst)
  - 输出边集 + 总造价
```

### 4.4 拓扑排序

| #  | 文件                | 任务               | 负责  |
| -- | ------------------- | ------------------ | ----- |
| 15 | `topological.h`   | ✅ 接口完成        | 组员B |
|    | `topological.cpp` | 🔲**待编码** | 组员B |

```
run_topological_sort(p_graph, out_seq, out_len, p_has_cycle)
  - Kahn 算法：计算入度 → 入度=0 入队 → 出队更新

has_cycle(p_graph)
  - 拓扑排序检测环（快捷接口）

print_topo_result()
  - 有环/无环分别输出
```

---

## 五、services/ 服务层

| #  | 文件                          | 任务                                              | 负责    |
| -- | ----------------------------- | ------------------------------------------------- | ------- |
| 16 | `road_network.h` / `.cpp` | ✅ 接口完成 — 双存储结构同步，城市/道路增删改查 | 组员A+B |
| 17 | `congestion.h` / `.cpp`   | ✅ 接口完成— 权值修改备份、拥堵前后对比报告      | 组员C   |
| 18 | `comparator.h` / `.cpp`   | ✅ 接口完成 — 四种性能指标统计 + 对比结论       | 组员C   |
| 19 | `file_io.h` / `.cpp`      | ✅ 接口完成— TXT 保存/加载/自动加载              | 组员A+C |

---

## 六、ui/ 交互层

| #  | 文件                       | 任务                                                  | 负责  |
| -- | -------------------------- | ----------------------------------------------------- | ----- |
| 20 | `menu.h` / `.cpp`      | ✅ 接口完成— 9项主菜单 + 多级子菜单 + 流程调度       | 组员C |
| 21 | `formatter.h` / `.cpp` | ✅ 接口完成 — 分隔线、表格对齐、格式化输出          | 组员C |
| 22 | `validator.h` / `.cpp` | ✅ 接口完成— 整数/字符串/业务规则校验 + 安全输入函数 | 组员C |

---

## 七、test/ 测试层

| #  | 文件                        | 任务                                                 | 负责  |
| -- | --------------------------- | ---------------------------------------------------- | ----- |
| 23 | `test_cases.h` / `.cpp` | 🔲**待编码** — 23 个测试用例实现 + TestRunner | 组员C |

测试覆盖：

| 类别       | 数量 | 内容                                               |
| ---------- | ---- | -------------------------------------------------- |
| 路网构建   | 8    | 合法添加、重复、边界容量、删除不存在、自环、负权值 |
| 图遍历     | 3    | 不连通图 DFS、单顶点 BFS、空图                     |
| 最短路径   | 3    | 正常路径、无路径、Floyd vs Dijkstra 交叉验证       |
| 最小生成树 | 3    | 正常生成树、不连通图、Prim vs Kruskal 交叉验证     |
| 拓扑排序   | 3    | DAG、有环图、无向图错误调用                        |
| 文件 IO    | 3    | 保存加载回环验证、文件不存在、格式错误             |

---

## 八、docs/ 文档

| #  | 文件                   | 任务                                                          | 负责  |
| -- | ---------------------- | ------------------------------------------------------------- | ----- |
| 24 | `coding_standard.md` | ✅ 已完成                                                     | 组员A |
| 25 | `flowcharts.md`      | 🔲**待完成**                                            | 组员A |
| 26 | 课程设计报告           | 🔲**待编写** — 按 general_requipment.md 第九章分块撰写 | 全体  |

---

## 九、main.cpp 主入口

| #  | 文件         | 任务                                                                 | 负责  |
| -- | ------------ | -------------------------------------------------------------------- | ----- |
| 27 | `main.cpp` | 🔲**待完善** — 初始化路网 → 加载数据 → 启动菜单 → 退出清理 | 组员A |

---

## 十、工作量统计

| 负责            | 已完成                                                                             | 待编码                                                          | 主要工作                       |
| --------------- | ---------------------------------------------------------------------------------- | --------------------------------------------------------------- | ------------------------------ |
| **组员A** | types.h, defines.h, graph_base.h, main.cpp 框架, 全部文档                          | road_network 部分, file_io 基础, main.cpp 完善, 课程报告 A 部分 | 架构、类型、接口、文档         |
| **组员B** | 8 个算法 `.h`、4 个基础结构 `.h`、adj_matrix.h/.cpp 骨架、adj_list.h/.cpp 骨架 | 全部 `.cpp` 编码：邻接矩阵 + 邻接表 + 4基础结构 + 7图算法     | 所有核心算法和数据结构的实现   |
| **组员C** | menu/formatter/validator/congestion/comparator/test 共 8 个 `.h`                 | 全部 `.cpp` 编码 + 全系统联调 + 23 测试用例 + 课程报告 C 部分 | 交互、测试、异常处理、报告汇总 |

---

## 开发顺序建议

```
第1步: 基础数据结构 (queue.cpp → stack.cpp → union_find.cpp → priority_queue.cpp)
         ↓
第2步: 图存储实现 (adj_matrix.cpp → adj_list.cpp)
         ↓
第3步: 图算法实现 (traversal → shortest_path → spanning_tree → topological)
         ↓
第4步: 服务层 (road_network → file_io → congestion → comparator)
         ↓
第5步: UI层 (validator → formatter → menu)
         ↓
第6步: 测试 (test_cases.cpp → 全系统联调)
         ↓
第7步: 完善 main.cpp + 编写课程报告
```
