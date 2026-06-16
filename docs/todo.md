# 开发任务清单

> 当前阶段：核心框架 + 基础数据结构 + 图存储 + UI交互 已完成，待完成 9 个 .cpp 文件
>
> 完成度：已完成 10/19 个 .cpp ≈ 53%

---

## 一、✅ 已完成的模块

### 公共基础层

| 文件 | 说明 | 负责 |
|------|------|------|
| `common/types.h` | 全局类型定义：7结构体、4枚举、13错误码 | 组员A |
| `common/defines.h` | 宏定义：调试开关、安全内存宏、工具宏 | 组员A |

### 图存储层（447 + 493 行）

| 文件 | 说明 | 负责 |
|------|------|------|
| `graph/graph_base.h` | 图抽象基类，26 个纯虚接口 | 组员A |
| `graph/adj_matrix.h` / `.cpp` | 邻接矩阵：全部 20 个方法已实现 | 组员B |
| `graph/adj_list.h` / `.cpp` | 邻接表：全部 20 个方法 + 私有辅助函数已实现 | 组员B |

### 基础数据结构（STL 风格）

| 文件 | 行数 | 说明 | 负责 |
|------|------|------|------|
| `algorithms/queue.h` / `.cpp` | 75 行 | 循环队列：push/pop/front/empty/full/clear | 组员B |
| `algorithms/stack.h` / `.cpp` | 66 行 | 顺序栈：push/pop/top/empty/full/clear | 组员B |
| `algorithms/priority_queue.h` / `.cpp` | 172 行 | 最小堆：push/pop/top/decrease_key/sift_up/sift_down | 组员B |
| `algorithms/union_find.h` / `.cpp` | 83 行 | 并查集：find_root(路径压缩)/union_sets(按秩合并) | 组员B |

### UI 交互层（361 + 187 + 327 行）

| 文件 | 说明 | 负责 |
|------|------|------|
| `ui/menu.h` / `.cpp` | 9 项主菜单 + 多级子菜单 + 流程调度（子菜单回调暂用占位符） | 组员C |
| `ui/formatter.h` / `.cpp` | 分隔线/标题/表格对齐/格式化输出 | 组员C |
| `ui/validator.h` / `.cpp` | 整数/字符串/业务规则校验 + 安全输入函数 | 组员C |

### 文档

| 文件 | 说明 | 负责 |
|------|------|------|
| `docs/coding_standard.md` | 代码规范：命名/格式/注释/接口/内存/测试/Git/禁忌 | 组员A |
| `docs/flowcharts.md` | 15 张 Mermaid 流程图 | 组员A |
| `docs/design_topic.md` | 选题说明 | 组员A |
| `docs/general_requipment.md` | 总体要求 | — |
| `docs/todo.md` | 本文件 | 组员A |

---

## 二、🔲 待完成的工作

### 第 1 步：图算法 .cpp（4 个文件缺失）— 组员 B

#### 1.1 创建 `algorithms/traversal.cpp`

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

#### 1.2 创建 `algorithms/shortest_path.cpp`

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

#### 1.3 创建 `algorithms/spanning_tree.cpp`

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

#### 1.4 创建 `algorithms/topological.cpp`

```
run_topological_sort(graph, out_sequence, out_length, has_cycle)
  - Kahn 算法：入度表 + Queue

has_cycle(graph)
print_topo_result()
```

---

### 第 2 步：服务层 .cpp（4 个文件缺失）— 组员 A+B+C

#### 2.1 创建 `services/road_network.cpp`（组员 A+B）

```
- 构造函数（同时 new AdjMatrix + new AdjList）
- get_graph(type) / get_type()
- 城市管理 6 个方法（add_city / remove_city / has_city / get_city_name / get_city_count / list_all_cities）
  → 每个操作在双结构上同步执行
- 道路管理 6 个方法（add_road / remove_road / update_road_weight / get_road_weight / has_road / get_road_count）
  → 每个操作在双结构上同步执行
- print_network_overview() / print_both_structures()
```

#### 2.2 创建 `services/congestion.cpp`（组员 C）

```
- 构造/析构（分配/释放修改记录数组）
- set_congestion(from, to, weight)   // 备份原始权值 + 修改
- restore_all()                       // 遍历修改记录恢复
- list_modified_roads()               // 打印修改列表
- run_comparison(start)               // 拥堵前后分别运行 Dijkstra
- print_comparison_report(start)      // 输出对比报告
```

#### 2.3 创建 `services/comparator.cpp`（组员 C）

```
- 构造/析构
- measure_memory()           // get_performance_stats 获取两图内存
- measure_traverse_time()    // 两图运行 DFS+BFS 并计时
- measure_find_efficiency()  // 两图顶点查找统计比较次数
- measure_edge_query()       // 两图边查询统计
- run_full_comparison()      // 运行全部测试 + 输出对比表格
- print_conclusion()         // 输出推荐适用场景
```

#### 2.4 创建 `services/file_io.cpp`（组员 A+C）

```
- save_to_file(graph, filepath)    // std::ofstream 写入
  → 格式：图类型 城市数 道路数\n城市行...\n道路行...
- load_from_file(graph, filepath)  // std::ifstream 解析
- auto_load(graph)                 // 启动时自动加载默认文件
- data_file_exists()               // 检测文件存在
- set_default_path(path)           // 设置默认路径
```

---

### 第 3 步：测试层 .cpp（1 个文件缺失）— 组员 C

#### 3.1 创建 `test/test_cases.cpp`

```
TestRunner 类 5 个方法：
  register_test() / run_all() / run_by_category() / print_report() / run_single()

23 个测试函数：
  路网构建 8：合法添加、重复、边界容量、删除不存在、自环、负权值
  图遍历 3：  不连通图 DFS、单顶点 BFS、空图
  最短路径 3：正常路径、无路径、Floyd vs Dijkstra 交叉验证
  MST 3：     正常生成树、不连通图、Prim vs Kruskal 交叉验证
  拓扑排序 3：DAG、有环图、无向图错误调用
  文件 IO 3： 保存加载回环验证、文件不存在、格式错误
```

---

### 第 4 步：完善 main.cpp — 组员 A

```
当前状态：已实现编码设置 + 菜单启动
待补充：
  - 检测历史数据文件是否存在（FileManager::data_file_exists）
  - 若存在 → 提示用户是否加载
  - 若不存在（或拒绝）→ 调用 load_default_data 初始化内置示例路网
  - 退出时清理 RoadNetwork 等资源
```

---

### 第 5 步：menu.cpp 联调 — 组员 C

```
当前状态：子菜单函数读取了用户输入并校验，但最终调用 print_service_unavailable()
待接入：
  - menu_network_edit()    → network->add_city() / add_road() 等
  - menu_traversal()       → traverse_dfs() / traverse_bfs()
  - menu_shortest_path()   → run_dijkstra() / run_floyd()
  - menu_spanning_tree()   → build_mst_prim() / build_mst_kruskal()
  - menu_topological_sort()→ run_topological_sort()
  - menu_congestion()      → congestion->set_congestion() / run_comparison()
  - menu_comparator()      → comparator->run_full_comparison()
  - menu_file_manage()     → FileManager::save_to_file() / load_from_file()
```

---

### 第 6 步：课程设计报告 — 全体

按 `general_requipment.md` 第九章分块撰写，组员 C 汇总排版。

| 章节 | 撰写人 |
|------|--------|
| 题目简介、需求分析、数据结构选型、整体架构、流程图 | 组员A |
| 详细设计、核心算法伪代码、算法复杂度分析 | 组员B |
| 测试用例、运行截图、BUG 修复记录、优缺点、源码附录 | 组员C |

---

## 三、工作量统计

| 角色 | 已完成 | 待完成 |
|------|--------|--------|
| **组员A** | types.h, defines.h, graph_base.h, coding_standard.md, flowcharts.md, main.cpp 部分 | road_network.cpp 部分, file_io.cpp 部分, main.cpp 完善, 报告 A 部分 |
| **组员B** | adj_matrix.cpp(447行), adj_list.cpp(493行), queue.cpp(75行), stack.cpp(66行), priority_queue.cpp(172行), union_find.cpp(83行) | traversal.cpp, shortest_path.cpp, spanning_tree.cpp, topological.cpp, road_network.cpp 部分, 报告 B 部分 |
| **组员C** | menu.cpp(361行), formatter.cpp(187行), validator.cpp(327行) | congestion.cpp, comparator.cpp, file_io.cpp 部分, test_cases.cpp, menu.cpp 联调, 报告 C 部分 |

---

## 四、开发顺序

```
✅ 第1步: 基础数据结构   (queue→stack→priority_queue→union_find)
✅ 第2步: 图存储实现      (adj_matrix→adj_list)
✅ 第3步: UI交互层        (validator→formatter→menu)
────────────────────────────────────────────────────
🔲 第4步: 图算法实现      (traversal→shortest_path→spanning_tree→topological)
🔲 第5步: 服务层          (road_network→file_io→congestion→comparator)
🔲 第6步: 测试层          (test_cases.cpp→全系统联调)
🔲 第7步: menu.cpp 联调   (占位符 → 真实服务/算法调用)
🔲 第8步: 完善 main.cpp   (数据文件检测加载)
🔲 第9步: 课程设计报告
```
