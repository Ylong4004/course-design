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

---

## 目录结构

```
course-design/
├── docs/
│   ├── design_topic.md              # 选题说明
│   ├── general_requipment.md        # 总体要求（12章）
│   ├── coding_standard.md           # 代码编写规范（8章：命名/格式/注释/接口/内存/测试/Git/禁忌）
│   └── flowcharts.md                # 流程图集（15张 Mermaid 图）
├── src/
│   ├── main.cpp                     # 程序入口
│   ├── common/
│   │   ├── types.h                  # 全局类型：7个结构体、4个枚举、13个错误码
│   │   └── defines.h                # 宏定义（调试开关/安全分配/工具宏）
│   ├── graph/
│   │   ├── graph_base.h             # 图抽象基类（26个纯虚接口）
│   │   ├── adj_matrix.h             # 邻接矩阵——类声明
│   │   ├── adj_matrix.cpp           # 邻接矩阵——实现骨架（待编码）
│   │   ├── adj_list.h               # 邻接表——类声明
│   │   └── adj_list.cpp             # 邻接表——实现骨架（待编码）
│   ├── algorithms/
│   │   ├── queue.h                  # 手写循环队列（BFS / Kahn 拓扑排序）
│   │   ├── stack.h                  # 手写顺序栈（DFS 非递归 / 路径回溯）
│   │   ├── union_find.h             # 手写并查集（Kruskal 判环）
│   │   ├── priority_queue.h         # 手写最小堆优先队列（Dijkstra / Prim 优化）
│   │   ├── traversal.h              # DFS / BFS 图遍历
│   │   ├── shortest_path.h          # Dijkstra 单源 + Floyd 多源最短路径
│   │   ├── spanning_tree.h          # Prim / Kruskal 最小生成树
│   │   └── topological.h            # 拓扑排序 + 环路检测
│   ├── services/
│   │   ├── road_network.h           # 路网管理器（双存储结构同步）
│   │   ├── congestion.h             # 拥堵模拟器（修改权值 + 前后对比）
│   │   ├── comparator.h             # 结构性能对比器（矩阵 vs 邻接表）
│   │   └── file_io.h                # 文件读写管理器（TXT 保存/加载）
│   ├── ui/
│   │   ├── menu.h                   # 9项主菜单 + 多级子菜单
│   │   ├── formatter.h              # 统一输出格式化（分隔线/表格/对齐）
│   │   └── validator.h              # 输入校验器（整数/字符串/业务规则）
│   └── test/
│       └── test_cases.h             # 测试框架 + 23个预定义测试用例
├── data/                            # 运行时数据文件目录（TXT）
├── .gitignore                       # Git 忽略规则
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
| 成员变量 | `m_` + `snake_case` | `m_vertex_count`, `m_edge_count` |
| 指针变量 | `p_` + 主体名         | `p_adj_matrix`, `p_head_node`    |
| 全局变量 | `g_` + `snake_case` | `g_graph_list`                     |
| 常量     | `UPPER_CASE`          | `MAX_CITY_COUNT`, `INF_WEIGHT`   |
| 公共函数 | `snake_case`          | `add_city()`, `run_dijkstra()`    |
| 私有函数 | `snake_case`          | `init_matrix()`, `find_index()`  |
| 结构体   | `PascalCase` + `_t` | `City_t`, `Edge_t`               |
| 类       | `PascalCase`          | `GraphBase`, `AdjMatrix`         |
| 文件     | `snake_case`          | `graph_base.h`, `adj_matrix.cpp` |
| 缩进     | 4 空格，禁止 Tab        | —                                   |

> 详见 [docs/coding_standard.md](docs/coding_standard.md)

---

## 角色分工

| 角色                     | 负责目录                                                                                                   | 核心内容                                                       |
| ------------------------ | ---------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------- |
| **组员A** 架构设计 | `common/`、`src/main.cpp`、`graph/graph_base.h`、`services/road_network.h`、`services/file_io.h` | 全局类型定义、抽象接口、主框架、文件基础接口、流程图绘制       |
| **组员B** 算法核心 | `graph/adj_matrix.*`、`graph/adj_list.*`、`algorithms/*`                                             | 双存储结构实现、4种基础数据结构 + 7大图算法全部手写            |
| **组员C** 交互测试 | `ui/*`、`services/congestion.*`、`services/comparator.*`、`test/*`                                 | 菜单系统、输入校验、输出格式化、拥堵模拟、性能对比、全系统测试 |

---

## 快速开始

1. 用 VSCode 或 Dev-C++ 打开 `src/` 目录
2. 编译所有 `.cpp` 文件（graph/ 目录下）
3. 运行可执行文件，进入主菜单
4. 首次运行自动加载示例路网数据

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
