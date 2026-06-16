# 城市交通路网分析系统

> **数据结构课程设计** | 三人协作 | 控制台应用程序
>
> 开发环境：Dev-C++ / VSCode | 语言：C/C++（仅标准库） | 版本管理：Git

---

## 项目概述

本系统面向区域城市交通网络开展建模与综合分析：

- 将城市抽象为**顶点**，道路抽象为**带权边**
- 同时使用**邻接矩阵** + **邻接表**两套存储结构
- 集成 **DFS、BFS、Dijkstra、Floyd、Prim、Kruskal、拓扑排序** 七大经典图算法
- 手写实现 **队列、栈、并查集、最小堆优先队列** 四种基础数据结构
- 支持路网编辑、最短路径规划、最小生成树、拥堵仿真、性能对比、数据持久化
- 23 个自动化测试用例（合法/非法/边界/交叉验证全覆盖）

---

## 目录结构

```
course-design/
├── docs/
│   ├── design_topic.md              # 选题说明
│   ├── general_requipment.md        # 总体要求（12章）
│   ├── coding_standard.md           # 代码编写规范（命名/格式/注释/接口/内存/测试/Git/禁忌）
│   ├── flowcharts.md                # 流程图集（15张 Mermaid 图）
│   └── todo.md                      # 开发任务清单
├── src/
│   ├── main.cpp                     # 程序入口（菜单模式 / 测试模式切换）
│   ├── common/
│   │   ├── types.h                  # 全局类型：7结构体、4枚举、13错误码
│   │   └── defines.h                # 宏：调试开关 + 安全内存（safe_new/delete）
│   ├── graph/
│   │   ├── graph_base.h             # 图抽象基类（26 纯虚接口）
│   │   ├── adj_matrix.h / .cpp      # 邻接矩阵实现
│   │   └── adj_list.h / .cpp        # 邻接表实现
│   ├── algorithms/
│   │   ├── queue.h / .cpp           # 循环队列（BFS / Kahn）
│   │   ├── stack.h / .cpp           # 顺序栈（DFS 非递归）
│   │   ├── union_find.h / .cpp      # 并查集（Kruskal 判环）
│   │   ├── priority_queue.h / .cpp  # 最小堆（Dijkstra / Prim 优化）
│   │   ├── traversal.h / .cpp       # DFS / BFS 图遍历
│   │   ├── shortest_path.h / .cpp   # Dijkstra + Floyd 最短路径
│   │   ├── spanning_tree.h / .cpp   # Prim + Kruskal 最小生成树
│   │   └── topological.h / .cpp     # 拓扑排序 + 环路检测
│   ├── services/
│   │   ├── road_network.h / .cpp    # 路网管理器（双存储同步）
│   │   ├── congestion.h / .cpp      # 拥堵模拟器
│   │   ├── comparator.h / .cpp      # 结构性能对比器
│   │   └── file_io.h / .cpp         # 文件读写（TXT）
│   ├── ui/
│   │   ├── menu.h / .cpp            # 9项主菜单 + 多级子菜单
│   │   ├── formatter.h / .cpp       # 输出格式化（表格/对齐）
│   │   └── validator.h / .cpp       # 输入校验
│   └── test/
│       ├── test_cases.h             # 测试框架声明
│       └── test_cases.cpp           # 23 个测试用例 + TestRunner
├── data/                            # 运行时数据文件（TXT）
├── .gitignore
└── README.md
```

---

## 架构设计

```
                    ┌─────────────┐
                    │   main.cpp  │  (组员A)
                    │  主入口/菜单 │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
         ┌────▼───┐  ┌────▼───┐  ┌────▼───┐
         │  UI层   │  │ 服务层  │  │ 测试层  │
         │(组员C)  │  │(A+B+C) │  │(组员C)  │
         └─────────┘  └───┬────┘  └─────────┘
                          │
              ┌───────────┼───────────┐
              │           │           │
         ┌────▼───┐ ┌────▼────┐ ┌───▼────┐
         │算法层   │ │GraphBase│ │基础结构│
         │(组员B) │ │(抽象接口)│ │(组员B) │
         │7大算法 │ │(组员A)  │ │队列/栈 │
         │        │ │        │ │并查集/堆│
         └─────────┘ └──┬──┬──┘ └────────┘
                        │  │
              ┌─────────┘  └─────────┐
              │                      │
        ┌─────▼─────┐         ┌─────▼─────┐
        │ AdjMatrix │         │  AdjList  │
        │  (组员B)  │         │  (组员B)  │
        └───────────┘         └───────────┘
```

**核心设计原则**：算法层仅依赖 `GraphBase*` 抽象接口，同一份算法代码可操作邻接矩阵和邻接表两种存储结构。

---

## 编码规范速查

| 类别     | 规则                    | 示例                                 |
| -------- | ----------------------- | ------------------------------------ |
| 局部变量 | `snake_case`          | `city_count`, `edge_weight`      |
| 成员变量 | `snake_case`          | `vertex_count`, `max_vertices`   |
| 常量     | `UPPER_CASE`          | `MAX_CITY_COUNT`, `INF_WEIGHT`   |
| 公共函数 | `snake_case`          | `add_city()`, `run_dijkstra()`   |
| 私有函数 | `snake_case`          | `init_matrix()`, `find_index()`  |
| 结构体   | `PascalCase` + `_t` | `City_t`, `Edge_t`               |
| 类       | `PascalCase`          | `GraphBase`, `AdjMatrix`         |
| 文件     | `snake_case`          | `graph_base.h`, `adj_matrix.cpp` |
| 缩进     | 4 空格，禁止 Tab        | —                                   |

> 详见 [docs/coding_standard.md](docs/coding_standard.md)

---

## 角色分工

| 角色                     | 负责目录                                                                                                   | 核心内容                                                                       |
| ------------------------ | ---------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------ |
| **组员A** 架构设计 | `common/`、`src/main.cpp`、`graph/graph_base.h`、`services/road_network.h`、`services/file_io.h` | 全局类型定义、抽象接口、主框架、文件基础接口、流程图绘制                       |
| **组员B** 算法核心 | `graph/adj_matrix.*`、`graph/adj_list.*`、`algorithms/*`                                             | 双存储结构实现、4 种基础数据结构 + 7 大图算法全部手写                          |
| **组员C** 交互测试 | `ui/*`、`services/congestion.*`、`services/comparator.*`、`test/*`                                 | 菜单系统、输入校验、输出格式化、拥堵模拟、性能对比、23 个测试用例 + 全系统联调 |

---

## 快速开始

### 编译

```bash
cd src
g++ -std=c++11 -Wall -O2 ^
    main.cpp ^
    graph/adj_matrix.cpp graph/adj_list.cpp ^
    algorithms/queue.cpp algorithms/stack.cpp ^
    algorithms/union_find.cpp algorithms/priority_queue.cpp ^
    algorithms/traversal.cpp algorithms/shortest_path.cpp ^
    algorithms/spanning_tree.cpp algorithms/topological.cpp ^
    services/road_network.cpp services/congestion.cpp ^
    services/comparator.cpp services/file_io.cpp ^
    ui/menu.cpp ui/formatter.cpp ui/validator.cpp ^
    test/test_cases.cpp ^
    -o main.exe
```

> `^` 是 cmd 的续行符，PowerShell 用 `` ` ``，Git Bash 用 `\`。

### 运行

ctrl+`打开工作区powershell，输入

```bash
./src/output/main.exe
```

进入 9 项主菜单，选择功能操作。

### 运行测试

编辑 `main.cpp`，去掉两行注释：

```cpp
// 改之前                       // 改之后
// run_all_tests();             run_all_tests();
// return 0;                    return 0;
```

重新编译运行，输出：

```
========== 全量测试 ==========
  测试: 添加城市(合法) ... ✓ 通过
  ...
---------- 测试报告 ----------
  总计: 23  |  通过: 23  |  失败: 0
==============================
```

### 调试模式

`src/common/defines.h` 第 12 行：

```cpp
#define DEBUG_MODE          // 去掉 // 开启 → 算法每步求解过程全打印
// #define DEBUG_MODE       // 加回 // 关闭 → 零开销
```

开启后重新编译，运行 Dijkstra 会输出每一步的顶点选择和松弛过程。

---

### Git 协作流程

```bash
# 开始
git pull
git checkout dev/member-{a,b,c}    # 切到自己的分支

# 写代码后提交
git add -A
git commit -m "完成 xxx 功能"

# 合并到 main
git checkout main
git pull
git merge dev/member-{a,b,c}
git push
```
