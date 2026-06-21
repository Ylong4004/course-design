# CHANGELOG

所有值得关注的变更均记录在本文件中。

格式基于 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.0.0/)，
版本号遵循 [语义化版本](https://semver.org/lang/zh-CN/)：大更新改中位，小更新改末位。

---

## [2.0.2] — 2026-06-21

### Changed

- **流程图迁移：Mermaid → Project Graph**：删除 `flowcharts.md`（688 行 Mermaid），替换为 5 个 `.prg`/`.md` 源文件 + 1 张截图
- `docs/prg/`：01_架构与流程图.md、02_任务流程.md、03_算法流程图.md、04_业务流程图.md、05_模块调用与数据结构.md
- 课程设计报告 §4.4：流程图描述改为 PRG 文件清单 + 截图引用
- README/todo/报告：所有"15 张 Mermaid"更新为"5 张 PRG 流程图"

---

## [2.0.1] — 2026-06-18

### Changed

- **README.md 重写快速开始章节**：去除 Qt 在线安装器（8GB）方案，改为 MSYS2 轻量方案（200MB）；g++ 直编作为默认方式
- **`main.cpp` 条件编译注释完善**：添加表格注释说明三种编译方式下 `--gui` 的行为差异

### Fixed

- **MSYS2 编译缺失 `mingw32-make`**：补充安装 `mingw-w64-ucrt-x86_64-make` 和 `mingw-w64-ucrt-x86_64-gcc`

---

## [2.0.0] — 2026-06-18

### Added

- **Qt GUI 可视化界面**（`src/qt/`，7 个模块）：
  - `qt_app`：主窗口、菜单栏、工具栏、状态栏、标签页布局
  - `network_view`：路网图可视化（顶点拖拽、滚轮缩放、路径高亮）
  - `city_panel`：城市增删面板
  - `road_panel`：道路增删改面板
  - `algorithm_panel`：算法选择与结果展示（遍历/最短路径/最小生成树/拓扑排序）
  - `congestion_panel`：拥堵模拟与对比面板
  - `compare_panel`：邻接矩阵 vs 邻接表性能对比面板
- **CMake 构建系统**（`CMakeLists.txt`）：
  - 自动检测 Qt5/Qt6，未安装 Qt 时仍可编译纯控制台版
  - 两个构建目标：`traffic_network`（含 GUI）、`traffic_console`（纯控制台）
- 启动参数 `--gui` / `-g` 进入 Qt 可视化界面模式

### Changed

- `main.cpp` 新增 Qt GUI 模式分派（`#ifdef QT_GUI_ENABLED`）
- `README.md` 更新：新增 CMake 编译说明、Qt6 安装指南、目录结构更新
- `.gitignore` 新增 `build/` 编译产物目录

### Fixed

- `network_view.cpp`：`QWheelEvent::delta()` → `angleDelta().y()`（Qt6 兼容）

---

## [1.9.2] — 2026-06-18

### Fixed

- CLI `save` 无参时默认保存到当前路网文件（不再写死 `default.txt`）
- CLI `topo` 执行失败时输出具体原因（无向图/错误码）
- CLI 退出自动保存路径同步修复

### Added

- CLI 文件路径追踪：`get_current_file()`/`set_current_file()`
- `docs/CHANGELOG.md`

---

## [1.9.1] — 2026-06-17

### Added

- CLI 新增 3 条命令：`list`、`congest_list`、`congest_report`
- CLI `save`/`load` 自动补全 `../data/` 前缀和 `.txt` 后缀

### Fixed

- CLI `menu` 命令导致程序直接退出 → 菜单↔CLI 循环切换
- CongestionSimulator 误传 `list_graph` → 修正为 `matrix_graph`
- CLI/批处理退出时未自动保存路网
- `tasks.json` 缺失 `cli/` 文件导致编译失败

---

## [1.9.0] — 2026-06-17

### Added

- **CLI 命令行模块**（`src/cli/`）：`cli_app` 入口模式分派 + `command_parser` 命令解析 + `commands.md` 速查手册
- 三种启动方式：无参数菜单模式、`--cli` 交互模式、单条命令批处理
- 菜单→CLI 切换（主菜单选项 10）

### Changed

- `main.cpp` 重构为 `cli_run(argc, argv)` 统一入口
- `menu.h` 新增 `switch_to_cli`、`requested_cli_switch()`、`get_network()` 等接口

---

## [1.8.0] — 2026-06-17

### Added

- 退出系统时自动保存到当前路网文件
- 文件名输入简化：只填文件名，自动加 `../data/` 前缀
- `MenuSystem` 新增 `current_file_path` 成员追踪当前文件
- `docs/数据结构课程设计报告.md` 初稿

---

## [1.7.0] — 2026-06-17

### Added

- **路网数据文件化管理**：`data/default.txt`，启动自动加载，支持多路网切换
- `print_network_detail()`：列出全部城市顶点和道路边详情
- 菜单「查看全部节点和路径」（路网编辑-选项 7）

### Changed

- `menu.cpp`：移除硬编码 `DemoCity`/`DemoRoad`，改为从文件加载
- 文件管理菜单新增「切换路网」「设置默认路径」
- 全项目 `#include <algorithm>` → 手写替代
- 4 个服务层文件：`namespace{}` → `static` 函数
- `defines.h` 精简：移除无用宏
- `.vscode/` 加入 `.gitignore`

### Fixed

- 菜单错误提示细化：区分重复编号/不存在/容量满/自环/负权值
- `std::to_string` → `std::cerr`（MinGW 兼容）
- `list_network_files` 用 `system+ifstream` 替代 `popen`

---

## [1.6.0] — 2026-06-16

### Added

- **23 个自动化测试用例**（`test/test_cases.cpp` + `TestRunner` 框架）
- **安全内存宏**：`safe_new`/`safe_delete`/`safe_new_array`/`safe_delete_array`
- **DEBUG_MODE 调试模式**：算法关键步骤输出（`defines.h` 开关控制）

### Changed

- 所有构造/析构：裸 `new`/`delete` → `safe_*` 宏
- 图算法层添加 `DEBUG_PRINT` 关键步骤输出
- `main.cpp`：支持菜单/测试模式切换

### Fixed

- 编译警告清零
- 文件 IO 测试路径适配

---

## [1.5.0] — 2026-06-16

### Added

- **图算法层 4 个文件**：`traversal.cpp`（DFS/BFS）、`shortest_path.cpp`（Dijkstra/Floyd）、`spanning_tree.cpp`（Prim/Kruskal）、`topological.cpp`（Kahn 算法）
- **服务层 4 个文件**：`road_network.cpp`（双存储同步）、`congestion.cpp`（拥堵模拟）、`comparator.cpp`（性能对比）、`file_io.cpp`（文件读写）

---

## [1.4.0] — 2026-06-15

### Added

- **UI 交互层 3 个文件**：`menu.cpp`（9 项主菜单）、`formatter.cpp`（格式化输出）、`validator.cpp`（输入校验）
- 9 项主菜单 + 多级子菜单 + 流程调度

---

## [1.3.0] — 2026-06-15

### Added

- **图存储双实现**：`adj_matrix.cpp`（447 行）、`adj_list.cpp`（493 行）全部 20+ 方法
- 邻接矩阵：`add_vertex`/`remove_vertex`/`add_edge`/`get_neighbors`/`get_performance_stats` 等
- 邻接表：链式存储、头插法、双向边处理、入边清理等

---

## [1.2.0] — 2026-06-15

### Added

- **基础数据结构 4 个**：`queue.cpp`（循环队列）、`stack.cpp`（顺序栈）、`priority_queue.cpp`（最小堆）、`union_find.cpp`（并查集）
- 全部 STL 风格（`push`/`pop`/`top` 直接返回值）

---

## [1.1.0] — 2026-06-15

### Changed

- 内存管理：`malloc`/`free` → `new`/`delete`
- I/O 层：`stdio` → `iostream`（`cout`/`cin`/`ofstream`/`ifstream`）
- 命名规范：全项目函数/变量统一 `snake_case`，`p_`/`m_` 前缀全移除
- 基础数据结构：错误码+输出参数 → STL 风格

### Fixed

- Windows 中文乱码：启动时 `system("chcp 65001 > nul")`

---

## [1.0.0] — 2026-06-15

### Added

- 项目框架搭建：`common/`、`graph/`、`algorithms/`、`services/`、`ui/`、`test/` 目录结构
- `types.h`：7 结构体、4 枚举、13 错误码
- `defines.h`：调试开关、安全宏、工具宏
- `graph_base.h`：26 个纯虚接口的图抽象基类
- 双存储结构骨架（`adj_matrix.h/.cpp`、`adj_list.h/.cpp`）
- 8 个算法头文件接口声明
- `docs/coding_standard.md`、`docs/flowcharts.md`、`docs/design_topic.md`、`docs/general_requipment.md`
