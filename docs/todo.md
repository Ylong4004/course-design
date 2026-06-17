# 开发任务清单

> 当前阶段：全部模块已完成，包括 CLI 命令行模块（24 条命令），待完成：课程设计报告
>
> 完成度：已完成 21/21 个 .cpp，1 个 CLI 文档，23/23 测试通过

---

## 一、已完成的模块

### 公共基础层

| 文件                 | 说明                                   | 负责  |
| -------------------- | -------------------------------------- | ----- |
| `common/types.h`   | 全局类型定义：7结构体、4枚举、13错误码 | 组员A |
| `common/defines.h` | 宏定义：调试开关、安全内存宏、工具宏   | 组员A |

### 图存储层（447 + 493 行）

| 文件                              | 说明                                        | 负责  |
| --------------------------------- | ------------------------------------------- | ----- |
| `graph/graph_base.h`            | 图抽象基类，26 个纯虚接口                   | 组员A |
| `graph/adj_matrix.h` / `.cpp` | 邻接矩阵：全部 20 个方法已实现              | 组员B |
| `graph/adj_list.h` / `.cpp`   | 邻接表：全部 20 个方法 + 私有辅助函数已实现 | 组员B |

### 基础数据结构（STL 风格）

| 文件                                       | 行数   | 说明                                                | 负责  |
| ------------------------------------------ | ------ | --------------------------------------------------- | ----- |
| `algorithms/queue.h` / `.cpp`          | 75 行  | 循环队列：push/pop/front/empty/full/clear           | 组员B |
| `algorithms/stack.h` / `.cpp`          | 66 行  | 顺序栈：push/pop/top/empty/full/clear               | 组员B |
| `algorithms/priority_queue.h` / `.cpp` | 172 行 | 最小堆：push/pop/top/decrease_key/sift_up/sift_down | 组员B |
| `algorithms/union_find.h` / `.cpp`     | 83 行  | 并查集：find_root(路径压缩)/union_sets(按秩合并)    | 组员B |

### UI 交互层（361 + 187 + 327 行）

| 文件                          | 说明                                                       | 负责  |
| ----------------------------- | ---------------------------------------------------------- | ----- |
| `ui/menu.h` / `.cpp`      | 9 项主菜单 + 多级子菜单 + 流程调度（子菜单回调暂用占位符） | 组员C |
| `ui/formatter.h` / `.cpp` | 分隔线/标题/表格对齐/格式化输出                            | 组员C |
| `ui/validator.h` / `.cpp` | 整数/字符串/业务规则校验 + 安全输入函数                    | 组员C |

### 文档

| 文件                           | 说明                                             | 负责  |
| ------------------------------ | ------------------------------------------------ | ----- |
| `docs/coding_standard.md`    | 代码规范：命名/格式/注释/接口/内存/测试/Git/禁忌 | 组员A |
| `docs/flowcharts.md`         | 15 张 Mermaid 流程图                             | 组员A |
| `docs/design_topic.md`       | 选题说明                                         | 组员A |
| `docs/general_requipment.md` | 总体要求                                         | —    |
| `docs/todo.md`               | 本文件                                           | 组员A |

---

## 二、已完成的工作

### 第 1 步：图算法 .cpp（4 个文件已完成）— 组员 B

#### 1.1  `algorithms/traversal.cpp`

```
traverse_dfs(graph, start, out_sequence, out_length)
  - 递归版：visited 数组标记 → 递归访问邻接点
  - 加分：用 Stack 实现非递归版

traverse_bfs(graph, start, out_sequence, out_length)
  - 用 Queue，起点入队 → 出队访问 → 邻接入队
  - 支持非连通图

print_traversal_sequence(graph, sequence, length, algo_name)
  - 顶点编号 → 城市名称，格式化输出序列
```

#### 1.2  `algorithms/shortest_path.cpp`

```
run_dijkstra(graph, start, out_dist, out_prev)
  - 基本版：O(V²) 数组
  - 加分：PriorityQueue 优化 O((V+E)logV)

dijkstra_get_path(prev, vertex_count, start, end, out_path, out_len)
  - 从 prev 数组回溯具体路径

run_floyd(graph, out_dist, out_next, vertex_count)
  - 三重循环 O(V³)

floyd_get_path(next, vertex_count, from, to, out_path, out_len)
  - next 矩阵回溯

print_dijkstra_result() / print_floyd_table()
  - 格式化输出
```

#### 1.3 `algorithms/spanning_tree.cpp`

```
build_mst_prim(graph, out_mst)
  - key 数组 + parent 数组 + inMST 标记
  - 基本版 O(V²)，加分：PriorityQueue 优化

build_mst_kruskal(graph, out_mst)
  - 收集边 → 排序 → UnionFind 判环
  - O(E log E)

print_mst_result(algo_name, mst)
free_mst_result(mst)
```

#### 1.4  `algorithms/topological.cpp`

```
run_topological_sort(graph, out_sequence, out_length, has_cycle)
  - Kahn 算法：入度表 + Queue

has_cycle(graph)
print_topo_result()
```

---

## 五、services/ 服务层

| #  | 文件                          | 任务                                                    | 负责    |
| -- | ----------------------------- | ------------------------------------------------------- | ------- |
| 16 | `road_network.h` / `.cpp` | ✅**已完成** — 双存储结构同步，城市/道路增删改查 | 组员A+B |
| 17 | `congestion.h` / `.cpp`   | ✅**已完成** — 权值修改备份、拥堵前后对比报告    | 组员C   |
| 18 | `comparator.h` / `.cpp`   | ✅**已完成** — 四种性能指标统计 + 对比结论       | 组员C   |
| 19 | `file_io.h` / `.cpp`      | ✅**已完成** — TXT 保存/加载/自动加载            | 组员A+C |

---

## 六、ui/ 交互层

| #  | 文件                       | 任务                                                          | 负责  |
| -- | -------------------------- | ------------------------------------------------------------- | ----- |
| 20 | `menu.h` / `.cpp`      | ✅**已完成** — 9项主菜单 + 多级子菜单 + 流程调度       | 组员C |
| 21 | `formatter.h` / `.cpp` | ✅**已完成** — 分隔线、表格对齐、格式化输出            | 组员C |
| 22 | `validator.h` / `.cpp` | ✅**已完成** — 整数/字符串/业务规则校验 + 安全输入函数 | 组员C |

---

## 七、test/ 测试层

| #  | 文件                        | 任务                                                 | 负责  |
| -- | --------------------------- | ---------------------------------------------------- | ----- |
| 23 | `test_cases.h` / `.cpp` | ✅**已完成** — 23 个测试用例实现 + TestRunner | 组员C |

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
| 25 | `flowcharts.md`      | ✅**已完成**                                            | 组员A |
| 26 | 课程设计报告           | 🔲**待编写** — 按 general_requipment.md 第九章分块撰写 | 全体  |

---

## 九、main.cpp 主入口

| 章节                                               | 撰写人 |
| -------------------------------------------------- | ------ |
| 题目简介、需求分析、数据结构选型、整体架构、流程图 | 组员A  |
| 详细设计、核心算法伪代码、算法复杂度分析           | 组员B  |
| 测试用例、运行截图、BUG 修复记录、优缺点、源码附录 | 组员C  |

---

## 三、工作量统计

| 角色            | 已完成                                                                                                                                                                                                                     | 待完成                     |
| --------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | -------------------------- |
| **组员A** | types.h, defines.h, graph_base.h, coding_standard.md, flowcharts.md, main.cpp 部分, road_network.cpp 部分, file_io.cpp 部分                                                                                                | main.cpp 完善, 报告 A 部分 |
| **组员B** | adj_matrix.cpp(447行), adj_list.cpp(493行), queue.cpp(75行), stack.cpp(66行), priority_queue.cpp(172行), union_find.cpp(83行), traversal.cpp, shortest_path.cpp, spanning_tree.cpp, topological.cpp, road_network.cpp 部分 | 报告 B 部分                |
| **组员C** | menu.cpp(361行), formatter.cpp(187行), validator.cpp(327行), congestion.cpp, comparator.cpp, file_io.cpp 部分, test_cases.cpp                                                                                              | 报告 C 部分                |

---

## 四、开发顺序

```
✅ 第1步: 基础数据结构   (queue→stack→priority_queue→union_find)
✅ 第2步: 图存储实现      (adj_matrix→adj_list)
✅ 第3步: UI交互层        (validator→formatter→menu)
────────────────────────────────────────────────────
✅ 第4步: 图算法实现      (traversal→shortest_path→spanning_tree→topological)
✅ 第5步: 服务层          (road_network→file_io→congestion→comparator)
✅ 第6步: 测试层          (test_cases.cpp→全系统联调)
✅ 第7步: menu.cpp 联调   (占位符 → 真实服务/算法调用)
🔲 第8步: 完善 main.cpp   (数据文件检测加载)
🔲 第9步: 课程设计报告
```
