# Qt GUI 新增方案可行性分析

> 根据 `docs/coding_standard.md` 规范编写，遵循项目模块化设计原则
>
> 编写人：组员C
> 日期：2026-06-17

---

## 一、可行性结论

**方案可行。** 现有服务/图/算法层与 UI 层已充分解耦，Qt GUI 前端可以**完全不修改任何现有业务代码**直接接入。

---

## 二、现有架构解耦程度分析

### 2.1 可直接复用的模块（零修改）

| 模块                   | 文件                                                                                    | 复用方式                                                     |
| ---------------------- | --------------------------------------------------------------------------------------- | ------------------------------------------------------------ |
| **公共类型**     | `common/types.h`                                                                      | 所有结构体、枚举、错误码直接用于 Qt                          |
| **图抽象接口**   | `graph/graph_base.h`                                                                  | Qt 通过 `GraphBase*` 操作，不感知存储细节                  |
| **图存储实现**   | `graph/adj_matrix.cpp`、`adj_list.cpp`                                              | 所有 CRUD 方法无控制台 I/O                                   |
| **基础数据结构** | `algorithms/queue.*`、`stack.*`、`priority_queue.*`、`union_find.*`             | 纯数据容器，完全独立                                         |
| **核心算法**     | `algorithms/traversal.*`、`shortest_path.*`、`spanning_tree.*`、`topological.*` | 全部通过 `GraphBase*` 和 out 参数输出                      |
| **路网管理**     | `services/road_network.*`                                                             | `add_city/remove_city/add_road/remove_road/...` 全部纯逻辑 |
| **拥堵模拟**     | `services/congestion.*`                                                               | `set_congestion/restore_all/run_comparison` 纯逻辑         |
| **性能对比**     | `services/comparator.*`                                                               | 四个 `measure_*` 方法通过 out 参数输出数值                 |
| **文件 I/O**     | `services/file_io.*`                                                                  | `save_to_file/load_from_file` 仅依赖 `<fstream>`         |

### 2.2 Qt 层需要绕开的方法（仅输出型方法，不涉及数据逻辑）

这些方法直接写 `std::cout`，Qt 不能调用，但**不影响系统功能**——Qt 自己负责展示，不需要它们：

| 方法                          | 所属类                  | Qt 替代方案                                                 |
| ----------------------------- | ----------------------- | ----------------------------------------------------------- |
| `print_network_overview()`  | `RoadNetwork`         | Qt 用 `get_city_count()` + `get_road_count()` 自建视图  |
| `print_network_detail()`    | `RoadNetwork`         | Qt 遍历 `get_all_vertex_ids()` + `get_neighbors()` 渲染 |
| `list_all_cities()`         | `RoadNetwork`         | Qt 调 `get_all_vertex_ids()` 填充 QTableWidget            |
| `print_both_structures()`   | `RoadNetwork`         | 不需要（性能数据用 `Comparator::measure_*`）              |
| `run_full_comparison()`     | `StructureComparator` | Qt 调四个 `measure_*` 后自建对比视图                      |
| `print_conclusion()`        | `StructureComparator` | Qt 自建分析视图                                             |
| `print_comparison_report()` | `CongestionSimulator` | Qt 自建对比视图                                             |
| `list_modified_roads()`     | `CongestionSimulator` | Qt 自建拥堵道路列表                                         |
| `print_graph()`             | `GraphBase` / 派生类  | 不调用                                                      |
| `print_*` 系列              | 各算法模块              | Qt 取算法返回值，自建可视化                                 |

> **关键结论**：上述方法全是"把数据格式化输出到控制台"的视图层代码。Qt 有自己更强大的视图能力，不需要它们。**数据访问层没有缺失任何一个 API。**

---

## 三、技术方案设计

### 3.1 新增目录结构

```
src/
├── qt/                          # 新增 Qt GUI 模块
│   ├── qt_main.h / .cpp         # Qt 入口 + 主窗口
│   ├── city_panel.h / .cpp      # 城市管理面板（增删改查）
│   ├── road_panel.h / .cpp      # 道路管理面板
│   ├── network_view.h / .cpp    # 路网可视化（图形绘制）
│   ├── algorithm_panel.h / .cpp # 算法执行面板（DFS/BFS/Dijkstra/Floyd/Prim/Kruskal）
│   ├── congestion_panel.h / .cpp# 拥堵模拟面板
│   ├── compare_panel.h / .cpp   # 性能对比面板
│   ├── file_panel.h / .cpp      # 文件管理面板
│   └── resources/               # Qt 资源文件（图标等）
├── main.cpp                     # 修改：新增 --gui / -g 启动方式
├── cli/                         # 不变
├── ui/                          # 不变
├── services/                    # **完全不改**
├── graph/                       # **完全不改**
├── algorithms/                  # **完全不改**
└── common/                      # **小清理：移除不必要的 #include <iostream>**
```

### 3.2 main.cpp 修改（约 5 行新增）

```cpp
// main.cpp，现有三种模式后新增：
if (std::strcmp(argv[1], "--gui") == 0 ||
    std::strcmp(argv[1], "-g") == 0) {
    qt_run(argc, argv);  // 启动 Qt GUI
}
```

启动方式扩展为四种：

```bash
program.exe                        # 菜单模式
program.exe --cli / -c             # CLI 交互模式
program.exe --gui / -g             # Qt GUI 模式（新增）
program.exe <命令> <参数...>        # 批处理模式
```

### 3.3 Qt 主窗口布局设计

```
┌──────────────────────────────────────────────────┐
│  城市交通路网分析系统              [_] [□] [X]    │
├──────────┬───────────────────────────────────────┤
│ 工具栏   │                                       │
│ ┌──────┐ │        路网可视化区域                  │
│ │新增城市│ │        (QGraphicsView)                │
│ │删除城市│ │                                       │
│ │新增道路│ │     城市节点 + 道路连线 + 权值标注     │
│ │删除道路│ │                                       │
│ │...    │ │                                       │
│ └──────┘ │                                       │
│          │                                       │
│ ┌──────┐ │                                       │
│ │DFS   │ │                                       │
│ │BFS   │ │                                       │
│ │Dijkstra│                                      │
│ │Floyd │ │                                       │
│ │Prim  │ │                                       │
│ │Kruskal│ │                                       │
│ │拓扑  │ │                                       │
│ └──────┘ │                                       │
├──────────┴───────────────────────────────────────┤
│  状态栏：城市数: 5  |  道路数: 8  |  类型: 无向图  │
└──────────────────────────────────────────────────┘
```

### 3.4 面板功能映射

| Qt 面板    | 调用的底层 API                                                                                            |
| ---------- | --------------------------------------------------------------------------------------------------------- |
| 城市管理   | `RoadNetwork::add_city/remove_city/has_city`                                                            |
| 道路管理   | `RoadNetwork::add_road/remove_road/update_road_weight/has_road`                                         |
| 路网可视化 | `RoadNetwork::get_all_vertex_ids` + `get_neighbors` + `get_edge_weight`                             |
| 算法面板   | 直接调 `traverse_dfs/bfs`、`run_dijkstra/floyd`、`build_mst_prim/kruskal`、`run_topological_sort` |
| 拥堵面板   | `CongestionSimulator::set_congestion/restore_all/run_comparison`                                        |
| 对比面板   | `StructureComparator::measure_memory/measure_traverse_time/measure_find_efficiency/measure_edge_query`  |
| 文件面板   | `FileManager::save_to_file/load_from_file/data_file_exists`                                             |

### 3.5 技术选型

| 项目               | 选择                                | 原因                      |
| ------------------ | ----------------------------------- | ------------------------- |
| **Qt 版本**  | Qt 5.15 或 Qt 6.x                   | MinGW 兼容，LTS 支持      |
| **图形框架** | QGraphicsView + QGraphicsScene      | 最适合节点-连线式路网渲染 |
| **表格**     | QTableWidget / QTableView           | 算法结果展示              |
| **布局**     | QSplitter + QVBoxLayout/QHBoxLayout | 可调整大小的面板布局      |
| **数据模型** | 直接调 RoadNetwork API              | 无需额外抽象层            |

---

## 四、工作量估算

| 阶段            | 内容                                                                     | 预估工作量          |
| --------------- | ------------------------------------------------------------------------ | ------------------- |
| **第1步** | 搭建 Qt 项目框架（CMakeLists.txt 或 .pro 文件），main.cpp 增加 `--gui` | 0.5 天              |
| **第2步** | 主窗口框架 + 工具栏 + 状态栏                                             | 1 天                |
| **第3步** | 城市管理面板（增删改查表单 + 列表）                                      | 1 天                |
| **第4步** | 道路管理面板                                                             | 0.5 天              |
| **第5步** | 路网可视化（QGraphicsView 画节点+连线）                                  | 1.5 天              |
| **第6步** | 算法面板（DFS/BFS/Dijkstra/Floyd/Prim/Kruskal/拓扑）                     | 1.5 天              |
| **第7步** | 拥堵模拟面板 + 对比报告面板                                              | 1 天                |
| **第8步** | 文件管理面板（保存/加载）                                                | 0.5 天              |
| **第9步** | 整合联调 + 测试                                                          | 1 天                |
| **合计**  |                                                                          | **约 8.5 天** |

---

## 五、风险评估

| 风险                                   | 等级 | 缓解措施                                          |
| -------------------------------------- | ---- | ------------------------------------------------- |
| **Qt 库体积大**（~30MB DLL）     | 低   | 仅 GUI 模式需要，菜单/CLI 模式零依赖              |
| **编码规范限制**（"无第三方库"） | 中   | Qt 是底层系统库级别的特例，且原有三种模式不受影响 |
| **MinGW 与 Qt 版本兼容**         | 低   | 已有 MinGW 8.1.0，安装对应 Qt 预编译包即可        |
| **学习曲线**（组员需学 Qt）      | 低   | Qt Widgets API 直观，信号/槽类似回调              |
| **编译时间增加**                 | 低   | Qt 模式单独编译，不拖慢菜单/CLI 模式              |

---

## 六、编码规范遵循说明

按照 `docs/coding_standard.md` 的要求：

| 规范项               | Qt 模块遵循方式                                                             |
| -------------------- | --------------------------------------------------------------------------- |
| **命名规范**   | Qt 类用大驼峰（`CityPanel`），Qt 变量用 `snake_case`（成员前缀 `m_`） |
| **文件命名**   | `city_panel.h/.cpp` 小写+下划线                                           |
| **缩进**       | 4 空格，K&R 括号风格                                                        |
| **注释**       | Doxygen 格式（`@brief/@param/@return`），中文注释                         |
| **接口规范**   | Qt 模块不暴露内部成员，通过公有方法对外                                     |
| **内存管理**   | Qt 父子对象树自动管理（`new QWidget(parent)`），无需手动 delete           |
| **安全内存宏** | Qt 窗口用 `new (std::nothrow)`，或用 Qt 的 `QPointer`                   |

---

## 七、建议与结论

**建议启动 Qt GUI 开发**，理由：

1. **零风险** — 不改任何现有代码，现有三种模式丝毫不受影响
2. **架构现成** — 服务层 API 已经完整，Qt 只是换个前端
3. **价值明显** — 路网可视化（真正看到节点和连线）是控制台无法提供的
4. **工作量可控** — 约 8.5 天，核心功能可进一步精简

如果同意本方案，下一步可进入详细设计阶段，并按第 3.1 节目录结构逐模块实现。
