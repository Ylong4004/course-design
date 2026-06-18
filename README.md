# 城市交通路网分析系统

> **数据结构课程设计** | 三人协作 | 控制台 + Qt GUI
>
> 开发环境： VSCode | 语言：C/C++ | 构建：CMake + MinGW | 版本管理：Git

---

## 项目概述

- 将城市抽象为**顶点**，道路抽象为**带权边**
- 同时使用**邻接矩阵** + **邻接表**两套存储结构
- 集成 **DFS、BFS、Dijkstra、Floyd、Prim、Kruskal、拓扑排序** 七大经典图算法
- 手写实现 **队列、栈、并查集、最小堆优先队列** 四种基础数据结构
- 支持路网编辑、最短路径规划、最小生成树、拥堵仿真、性能对比、数据持久化
- 23 个自动化测试用例（合法/非法/边界/交叉验证全覆盖）
- **路网数据文件化管理**：程序启动自动加载，支持多路网切换

---

## 目录结构

```
course-design/
├── CMakeLists.txt                   # CMake 构建文件（支持 Qt5/Qt6 + 纯控制台）
├── docs/
│   ├── design_topic.md              # 选题说明
│   ├── general_requipment.md        # 总体要求（12章）
│   ├── coding_standard.md           # 代码编写规范
│   ├── flowcharts.md                # 流程图集（15张 Mermaid 图）
│   └── todo.md                      # 开发任务清单
├── src/                             # 源代码
│   ├── main.cpp                     # 程序入口（菜单/CLI/Qt/批处理）
│   ├── common/        types.h  defines.h
│   ├── graph/         graph_base.h  adj_matrix.*  adj_list.*
│   ├── algorithms/    8 个模块：4 数据结构 + 4 算法
│   ├── services/      4 个模块：路网/拥堵/对比/文件
│   ├── ui/            3 个模块：菜单/格式化/校验
│   ├── cli/           2 个模块：命令行交互
│   ├── qt/            7 个模块：Qt GUI 界面（可选）
│   └── test/          test_cases.* (23 个测试用例)
├── build/                           # CMake 编译产物（git 忽略）
│   └── bin/           traffic_network.exe / traffic_console.exe
├── data/                            # 路网数据文件（.txt）
│   └── default.txt                  # 默认路网
├── .gitignore
└── README.md
```

---

## 功能与使用说明

### 1. 路网编辑（主菜单 1）

```
 1. 新增城市     → 输入编号 + 名称
 2. 删除城市     → 输入编号（同步删除关联道路）
 3. 新增道路     → 输入起点、终点、权值
 4. 删除道路     → 输入起点、终点
 5. 修改道路权值 → 输入起点、终点、新权值
 6. 查看路网总览 → 图类型、城市数、道路数
 7. 查看全部节点和路径 → 列出所有城市和道路的详细信息
```

> 删除城市会自动清理所有关联道路。所有修改在邻接矩阵和邻接表上同步执行。

### 2. 图遍历（主菜单 2）

```
 1. DFS 深度优先遍历 → 输入起点，输出访问序列
 2. BFS 广度优先遍历 → 输入起点，输出访问序列
```

### 3. 最短路径（主菜单 3）

```
 1. Dijkstra 单源最短路径 → 输入起点，输出到所有城市的最短距离
 2. Floyd 多源最短路径   → 输出任意两城间的最短距离对照表
```

### 4. 最小生成树（主菜单 4）

```
 1. Prim 算法   → 构建最小生成树，输出边集和总造价
 2. Kruskal 算法 → 同上（两种算法结果一致，验证正确性）
```

> 仅适用于无向图。有向图会自动提示。

### 5. 拓扑排序（主菜单 5）

> 仅适用于有向图。无向图会提示"无法执行"。

### 6. 拥堵模拟（主菜单 6）

```
 1. 修改道路权值       → 指定道路 + 新权值（模拟拥堵）
 2. 恢复全部修改       → 撤销所有拥堵修改
 3. 查看修改记录       → 显示当前拥堵的道路和权值变化
 4. 拥堵前后对比分析   → 自动对比最优路径变化
```

### 7. 结构性能对比（主菜单 7）

自动统计邻接矩阵 vs 邻接表在四项指标上的差异：

- 内存占用
- 遍历速度
- 顶点查找效率
- 边查询效率

### 8. 数据文件管理（主菜单 8）

```
 1. 保存路网到文件        → 输入文件名，保存为 data/xxx.txt
 2. 从文件加载路网        → 输入文件名，替换当前路网
 3. 切换路网（从 data/ 选择） → 列出所有可用文件，选择后加载
 4. 设置默认文件路径      → 修改自动加载的默认文件
```

### 9. 帮助 / 关于（主菜单 9）

显示功能概览。

### 10. 命令行模式（主菜单 10 / --cli 启动）

菜单中选 10 或启动时加 `--cli` 进入。`menu` 切回菜单，`exit` 退出。

| 命令               | 参数                | 说明               |
| ------------------ | ------------------- | ------------------ |
| `new_city`       | `<id> <name>`     | 添加城市           |
| `del_city`       | `<id>`            | 删除城市           |
| `new_road`       | `<from> <to> <w>` | 添加道路           |
| `del_road`       | `<from> <to>`     | 删除道路           |
| `update_road`    | `<from> <to> <w>` | 修改权值           |
| `show_network`   | —                  | 路网详情           |
| `dfs`            | `<start>`         | 深度优先遍历       |
| `bfs`            | `<start>`         | 广度优先遍历       |
| `dijkstra`       | `<start> [end]`   | 最短路径           |
| `floyd`          | —                  | 全源最短路径表     |
| `prim`           | —                  | Prim 最小生成树    |
| `kruskal`        | —                  | Kruskal 最小生成树 |
| `topo`           | —                  | 拓扑排序           |
| `congest`        | `<from> <to> <w>` | 设置拥堵           |
| `restore`        | —                  | 恢复拥堵           |
| `congest_list`   | —                  | 查看拥堵记录       |
| `congest_report` | `<start>`         | 拥堵对比报告       |
| `compare`        | —                  | 结构性能对比       |
| `save`           | `[filename]`      | 保存路网           |
| `load`           | `[filename]`      | 加载路网           |
| `list`           | —                  | 列出可用路网文件   |
| `help`           | —                  | 命令帮助           |
| `menu`           | —                  | 切回菜单模式       |
| `exit`           | —                  | 退出               |

> 文件名自动补全 `../data/` 前缀和 `.txt` 后缀。

### 0. 退出系统

退出时自动保存到当前路网文件。

---

## 数据文件格式（TXT）

```
图类型  城市数  道路数          ← 图类型：0=无向图, 1=有向图
城市ID  城市名称               ← 每个城市一行
城市ID  城市名称
...
起点ID  终点ID  权值           ← 每条道路一行
起点ID  终点ID  权值
...
```

示例（`data/default.txt`）：

```
0 5 6
1 City1
2 City2
3 City3
4 City4
5 City5
1 2 10
1 3 18
2 3 6
2 4 14
3 5 7
4 5 9
```

---

## 快速开始

### 方式一：CMake 构建（推荐，支持 Qt GUI）

#### 1. 环境要求

| 工具 | 最低版本 | 说明 |
|------|---------|------|
| **CMake** | 3.16+ | [下载](https://cmake.org/download/)，或使用 Qt 安装时自带的 CMake |
| **MinGW-w64** | 8.0+ | GCC/G++ 编译器，推荐使用 Qt 自带的 MinGW 13.1 |
| **Qt 6**（可选） | 6.2+ | 仅 GUI 版本需要；不装 Qt 仍可编译纯控制台版 |

#### 2. 安装 Qt 6（仅 GUI 版本需要）

1. 下载 [Qt 在线安装器](https://www.qt.io/download-qt-installer-oss)
2. 安装时勾选：
   - **Qt 6.x** → 勾选 `MinGW 13.1.x 64-bit` 组件
   - **Developer and Designer Tools** → 勾选 `MinGW 13.1.x 64-bit`（Qt 自带的编译器）
3. 记住安装路径，例如 `D:\Qt`

> 安装完成后无需额外配置环境变量，CMake 会通过 `CMAKE_PREFIX_PATH` 自动查找 Qt。

#### 3. 编译

```powershell
# 在项目根目录下执行
# 将 D:\Qt 替换为你的实际 Qt 安装路径

cmake -S . -B build -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="D:\Qt\6.11.1\mingw_64" ^
    -DCMAKE_BUILD_TYPE=Release

cmake --build build --target traffic_network    # 含 Qt GUI 版本
cmake --build build --target traffic_console    # 纯控制台版本（不依赖 Qt）
```

> **⚠ 路径含中文的解决办法**：MinGW 的 MOC 工具不支持中文路径，编译前需执行：
> ```
> subst T: <项目绝对路径>       # 将项目映射到 T: 盘
> # 然后在 T:\ 下执行上述 cmake 命令
> subst T: /D                   # 编译完成后释放虚拟盘
> ```

#### 4. 运行

编译产物在 `build/bin/` 目录下：

```powershell
# Qt 可视化界面模式
.\build\bin\traffic_network.exe --gui

# 菜单模式（默认）
.\build\bin\traffic_console.exe

# 命令行交互模式
.\build\bin\traffic_console.exe --cli
```

---

### 方式二：g++ 直接编译（仅控制台，无需 CMake 和 Qt）

```bash
cd ./src
g++ -std=c++11 -Wall -g3 main.cpp graph/adj_matrix.cpp graph/adj_list.cpp algorithms/queue.cpp algorithms/stack.cpp algorithms/union_find.cpp algorithms/priority_queue.cpp algorithms/traversal.cpp algorithms/shortest_path.cpp algorithms/spanning_tree.cpp algorithms/topological.cpp services/road_network.cpp services/congestion.cpp services/comparator.cpp services/file_io.cpp ui/menu.cpp ui/formatter.cpp ui/validator.cpp test/test_cases.cpp cli/cli_app.cpp cli/command_parser.cpp -o output/main.exe
```

> VSCode 中按 `Ctrl+Shift+B` → 选"编译全部（21个cpp）"即可。

### 运行

```bash
cd ./src

# 菜单模式（无参数，默认）
./output/main.exe

# 命令行交互模式
./output/main.exe --cli

# 单条命令批处理
./output/main.exe dijkstra 1 5
```

### 运行测试

编辑 `main.cpp`，把 `// run_all_tests();` 和 `// return 0;` 前面的 `//` 去掉，重新编译运行。

### 调试模式

`src/common/defines.h` 第 12 行，去掉 `//` 开启：

```cpp
#define DEBUG_MODE    // 开启 → 算法每一步求解过程全打印
```

### 新建路网

1. 在 `data/` 下新建 `.txt` 文件（按格式写顶点和边）
2. 启动程序 → 主菜单 8 → 3 切换路网 → 选择新文件
3. 或在程序中手动添加城市/道路后 → 主菜单 8 → 1 保存

---

## 编码规范速查

| 类别     | 规则                    | 示例                                 |
| -------- | ----------------------- | ------------------------------------ |
| 局部变量 | `snake_case`          | `city_count`, `start_city`       |
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

## Git 协作

```bash
git pull
git checkout dev/member-{a,b,c}
git add -A
git commit -m "完成 xxx 功能"
git checkout main
git pull
git merge dev/member-{a,b,c}
git push
```
