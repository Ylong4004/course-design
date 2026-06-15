# 代码编写规范

> 适用于《数据结构》课程设计——城市交通路网分析系统
>
> 参考依据：`general_requipment.md` 第四章

---

## 一、命名规范

### 1.1 变量命名

变量（除常量外）统一使用 **小写 + 下划线**（snake_case），前缀另加：

| 类型     | 规范                          | 示例                                            |
| -------- | ----------------------------- | ----------------------------------------------- |
| 局部变量 | `snake_case`（小写+下划线） | `city_count`, `edge_weight`, `is_visited` |
| 全局变量 | `snake_case`                | `graph_matrix`, `graph_list`                |
| 成员变量 | `snake_case`                | `vertex_num`, `edge_num`                    |
| 常量     | 全大写 + 下划线               | `MAX_CITY_COUNT`, `INF_WEIGHT`              |
| 指针变量 | `snake_case`                | `adj_matrix`, `head_node`                   |

### 1.2 函数命名

| 类型          | 规范           | 示例                                     |
| ------------- | -------------- | ---------------------------------------- |
| 公共接口      | `snake_case` | `add_city()`, `find_shortest_path()` |
| 私有/内部函数 | `snake_case` | `init_matrix()`, `resize_list()`     |
| 算法函数      | `snake_case` | `run_dijkstra()`, `build_mst_prim()` |

### 1.3 结构体/类命名

| 类型   | 规范                | 示例                                      |
| ------ | ------------------- | ----------------------------------------- |
| 结构体 | 大驼峰 +`_t` 后缀 | `City_t`, `Edge_t`, `PathResult_t`  |
| 类     | 大驼峰              | `GraphBase`, `AdjMatrix`, `AdjList` |

### 1.4 文件命名

| 类型   | 规范          | 示例                               |
| ------ | ------------- | ---------------------------------- |
| 头文件 | 小写 + 下划线 | `graph_base.h`, `adj_matrix.h` |
| 源文件 | 小写 + 下划线 | `adj_matrix.cpp`, `main.cpp`   |

---

## 二、代码格式

### 2.1 缩进

- 统一使用 **4 空格** 缩进（禁止 Tab）
- VSCode 设置：`"editor.insertSpaces": true`, `"editor.tabSize": 4`

### 2.2 括号风格

使用 **K&R 风格**（左括号不换行）：

```cpp
if (condition) {
    // code
}

void FunctionName() {
    // code
}
```

### 2.3 每行长度

- 单行不超过 **80 字符**
- 超长参数列表对齐换行：

```cpp
void add_road(GraphBase* p_graph,
              int from_city,
              int to_city,
              int weight,
              bool is_directed);
```

### 2.4 空行规则

- 函数之间空 1 行
- 逻辑块之间空 1 行
- `#include` 区段与代码之间空 1 行

---

## 三、注释要求

### 3.1 文件头注释（每个 .h / .cpp 必须）

```cpp
/*
 * 模块名称  : 图存储——邻接表实现
 * 编写人    : 组员B（算法&核心功能负责人）
 * 功能描述  : AdjList 成员函数实现骨架（待编码）
 */
```

### 3.2 结构体注释

```cpp
/**
 * @brief 城市顶点结构体
 * @field id       城市编号（唯一）
 * @field city_name 城市名称
 */
typedef struct {
    int  id;
    char city_name[32];
} City_t;
```

### 3.3 函数注释（公共接口必须）

```cpp
/**
 * @brief  使用 Dijkstra 算法计算单源最短路径
 * @param  p_graph     图存储对象指针（邻接矩阵或邻接表）
 * @param  start_city  起点城市编号
 * @param  out_dist    输出：到各城市的最短距离数组
 * @param  out_prev    输出：前驱结点数组（用于回溯路径）
 * @return 成功返回 0，失败返回错误码
 * @note   时间复杂度 O(V²)，V 为顶点数
 */
int run_dijkstra(const GraphBase* p_graph,
                int start_city,
                int out_dist[],
                int out_prev[]);
```

### 3.4 行内注释

```cpp
// 跳过已访问的城市顶点
if (visited[i]) {
    continue;
}

// 松弛操作：尝试通过当前结点更新邻居距离
if (dist[u] + weight < dist[v]) {
    dist[v] = dist[u] + weight;
}
```

### 3.5 Doxygen 文档注释标签

本项目使用 **Doxygen 标准标签** 编写结构化注释，以下为常用标签说明：

#### 类/结构体相关

| 标签       | 含义              | 用法                                      |
| ---------- | ----------------- | ----------------------------------------- |
| `@class` | 标记类/结构体说明 | `@class 类名` — 注明这是一个类的注释块 |
| `@brief` | 一句话简述        | `@brief 图存储结构抽象基类`             |
| `@field` | 结构体/类成员字段 | `@field id  城市唯一编号`               |

#### 函数相关

| 标签        | 含义               | 用法                                                  |
| ----------- | ------------------ | ----------------------------------------------------- |
| `@brief`  | 函数功能一句话描述 | `@brief 使用Dijkstra算法计算单源最短路径`           |
| `@param`  | 参数说明           | `@param pGraph  图存储对象指针（邻接矩阵或邻接表）` |
| `@return` | 返回值说明         | `@return 成功返回 SUCCESS，失败返回错误码`          |
| `@note`   | 补充说明/注意事项  | `@note 时间复杂度 O(V²)，适用于非负权图`           |

#### 测试相关

| 标签        | 含义         | 用法                                              |
| ----------- | ------------ | ------------------------------------------------- |
| `@test`   | 测试用例描述 | `@test 测试用例：添加重复城市`                  |
| `@expect` | 预期结果     | `@expect 返回 ERR_DUPLICATE_CITY，城市列表不变` |

#### 完整示例

```cpp
/**
 * @class GraphBase
 * @brief 图存储结构抽象基类
 *
 * 所有图存储结构（邻接矩阵、邻接表）必须实现以下纯虚接口。
 * 上层算法通过 GraphBase* 指针操作，无需感知底层存储细节。
 */
class GraphBase {
public:
    /**
     * @brief  删除指定城市及关联的所有道路
     * @param  city_id  城市编号
     * @return 成功返回 SUCCESS，失败返回 ERR_CITY_NOT_FOUND
     * @note   会同步删除以该城市为端点的所有道路边
     */
    virtual int RemoveVertex(int city_id) = 0;
};
```

> **核心原则**：让人和工具都能快速理解代码。组员调用接口时只看 `.h` 里的 `@brief` + `@param` + `@return` 就知道怎么用；VSCode 鼠标悬停会自动显示这些注释；Doxygen 工具可一键生成接口文档。

---

## 四、模块接口规范

### 4.1 设计原则

- 架构负责人（组员 A）定义 **`.h` 头文件接口**
- 各开发人员按接口实现 **`.cpp` 源文件**
- 模块间通过头文件声明的函数/类进行交互
- 禁止跨模块直接访问其他模块的内部变量

### 4.2 接口稳定原则

```cpp
// ✅ 正确：接口简洁，参数明确
int add_city(int id, const char* name);

// ❌ 错误：暴露内部结构
extern City_t g_city_array[];  // 不该让其他模块直接操作
```

### 4.3 错误码统一

```cpp
#define SUCCESS              0    // 操作成功
#define ERR_CITY_NOT_FOUND   1    // 城市不存在
#define ERR_ROAD_EXISTS      2    // 道路已存在
#define ERR_INVALID_INPUT    3    // 非法输入
#define ERR_FILE_OPEN_FAIL   4    // 文件打开失败
#define ERR_OUT_OF_MEMORY    5    // 内存不足
#define ERR_GRAPH_EMPTY      6    // 图为空
#define ERR_HAS_CYCLE        7    // 图中存在环路
```

---

## 五、内存管理

统一使用 `new` / `delete`（C++ 方式），禁止使用 `malloc` / `free`。

> **原因**：项目中有类对象（`AdjMatrix`、`AdjList` 等），必须用 `new`/`delete` 才能正确调用构造函数和析构函数。`malloc`/`free` 不调用构造/析构，会导致成员变量未初始化、内存泄漏。

### 5.1 分配与释放配套

| 分配方式                   | 释放方式         | 场景                                   |
| -------------------------- | ---------------- | -------------------------------------- |
| `new Type`               | `delete ptr`   | 单个对象                               |
| `new Type[n]`            | `delete[] ptr` | 数组                                   |
| `new(std::nothrow) Type` | `delete ptr`   | 单个对象（失败返回 nullptr，不抛异常） |

```cpp
// 分配单个对象
AdjMatrix* p_matrix_graph = new (std::nothrow) AdjMatrix(100, GRAPH_UNDIRECTED);
if (p_matrix_graph == nullptr) {
    std::cerr << "[错误] 内存分配失败！" << std::endl;
    return ERR_OUT_OF_MEMORY;
}

// 分配数组
int* p_buffer = new (std::nothrow) int[buf_size];
City_t* p_cities = new (std::nothrow) City_t[max_count];
int** p_matrix = new (std::nothrow) int*[rows];
for (int i = 0; i < rows; i++) {
    p_matrix[i] = new (std::nothrow) int[cols];
}

// 释放（与分配顺序相反）
for (int i = 0; i < rows; i++) {
    delete[] p_matrix[i];
}
delete[] p_matrix;
delete[] p_cities;
delete[] p_buffer;
delete p_matrix_graph;       // 单个对象用 delete
```

### 5.2 链表结点释放

```cpp
void free_edge_list(EdgeNode_t* p_head) {
    EdgeNode_t* p_curr = p_head;
    while (p_curr != nullptr) {
        EdgeNode_t* p_next = p_curr->p_next;
        delete p_curr;              // 单个结点用 delete
        p_curr = p_next;
    }
}
```

### 5.3 使用安全宏（推荐）

项目 `defines.h` 提供了四个安全宏，自动处理失败检查和置空：

```cpp
// 单个对象
safe_new(p_graph, AdjMatrix, 100, GRAPH_UNDIRECTED);   // 分配 + 失败检查
safe_delete(p_graph);                                   // 释放 + 置 nullptr

// 数组
safe_new_array(p_buffer, int, buf_size);                // 分配 + 失败检查
safe_delete_array(p_buffer);                            // 释放 + 置 nullptr
```

### 5.4 严禁混用

```cpp
// ❌ 致命错误——未定义行为
int* p = new int[100];
free(p);                        // new 出来的用 free 释放，崩

EdgeNode_t* n = new EdgeNode_t;
free(n);                        // 析构函数不会被调用

// ❌ 反过来也错
int* p2 = (int*)malloc(sizeof(int) * 100);
delete[] p2;                    // malloc 出来的用 delete 释放，崩
```

---

## 六、测试规范

### 6.1 测试用例模板

```cpp
/**
 * @test   测试用例：添加重复城市
 * @expect 返回 ERR_DUPLICATE_CITY，城市列表不变
 */
void Test_AddDuplicateCity() {
    // Arrange（准备）
    GraphBase* p_graph = CreateGraph(GRAPH_MATRIX);
    add_city(p_graph, 1, "北京");

    // Act（执行）
    int result = add_city(p_graph, 1, "南京");

    // Assert（断言）
    assert(result == ERR_DUPLICATE_CITY);
    DestroyGraph(p_graph);
}
```

### 6.2 三层测试覆盖

| 层次     | 负责         | 内容                         |
| -------- | ------------ | ---------------------------- |
| 单元测试 | 各模块负责人 | 函数级正确性验证             |
| 集成测试 | 组员 C       | 模块联调，合法/非法/边界数据 |
| 性能测试 | 组员 B       | 核心算法耗时、比较次数       |

---

## 七、Git 版本管理

项目使用 **Git** 进行版本控制，所有代码改动通过 Git 追踪，**禁止**手动备份 `xxx_YYYYMMDD.cpp` 这类文件。

### 7.1 基本工作流

```bash
# 查看当前状态（最常用，每次操作前先跑一遍）
git status

# 查看具体改了什么内容
git diff

# 查看某个文件的改动
git diff src/graph/adj_matrix.cpp

# 添加文件到暂存区
git add src/graph/adj_matrix.cpp          # 单个文件
git add src/graph/                         # 整个目录
git add -A                                 # 全部改动

# 提交（message 用中文简要说明做了什么）
git commit -m "完成邻接矩阵 AddVertex / AddEdge 实现"

# 查看提交历史
git log --oneline                          # 简洁版（一行一条）
git log --graph --all --oneline            # 带分支图的版本
```

### 7.2 分支协作

三人各自在自己的分支上开发，互不干扰，完成后合并到 main：

```bash
# 创建并切换到自己的分支
git checkout -b dev/member-b               # 组员B（算法）

# 在分支上正常开发、多次提交
git add src/algorithms/dijkstra.cpp
git commit -m "实现 Dijkstra 最短路径算法"

git add src/algorithms/floyd.cpp
git commit -m "实现 Floyd 多源最短路径"

# 切回 main 合并自己的分支
git checkout main
git merge dev/member-b

# 合并完成后可以删除自己的分支（可选）
git branch -d dev/member-b

# 推送到远端
git push
```

### 7.3 回退与撤销

```bash
# 撤销某个文件未暂存的修改（恢复到最近一次 commit 的状态）
git checkout -- src/graph/adj_matrix.cpp

# 撤销 git add（把文件从暂存区撤下来）
git reset HEAD src/graph/adj_matrix.cpp

# 撤销最近一次 commit，改动保留在工作区（重新修改后再提交）
git reset --soft HEAD~1

# 查看某个文件在历史上的所有变动
git log -p -- src/graph/adj_matrix.cpp
```

### 7.4 冲突处理

当两人同时改了同一个文件并合并时会产生冲突：

```bash
# 合并时提示 CONFLICT
git merge dev/member-c

# 打开冲突文件，会看到这样的标记：
# <<<<<<< HEAD
#   你的版本
# =======
#   对方的版本
# >>>>>>> dev/member-c

# 手动编辑保留正确内容，删除 <<<<<<< / ======= / >>>>>>> 标记
# 编辑完成后：
git add 冲突文件.cpp
git commit -m "解决合并冲突"
```

### 7.5 .gitignore 忽略规则

项目根目录必须创建 `.gitignore`，避免把编译产物、临时文件提交到仓库：

```bash
# 编译产物
*.exe
*.o
*.obj
*.out

# IDE 临时文件
.vscode/
*.dev
*.layout

# 系统文件
Thumbs.db
Desktop.ini

# 调试生成的路网数据（保留 data/ 目录结构，忽略内容）
data/*.txt
```

### 7.6 Commit message 规范

| 前缀     | 含义                 | 示例                                   |
| -------- | -------------------- | -------------------------------------- |
| `完成` | 新功能开发完毕       | `完成 Dijkstra 最短路径`             |
| `修复` | Bug 修复             | `修复 Floyd 路径回溯数组越界`        |
| `重构` | 优化代码结构不改功能 | `重构邻接表 AddEdge 去重逻辑`        |
| `文档` | 注释/文档更新        | `文档 更新 coding_standard Git 章节` |
| `合并` | 分支合并             | `合并 dev/member-c 交互测试模块`     |

### 7.7 标准操作

```bash
# 工作前
git pull                                  # 拉取最新代码

# 切到自己的分支
git checkout dev/member-b

# 写代码...

# 提交
git status                                # 看看改了什么
git add -A
git commit -m "完成 xxx 功能"

# 切回 main 合并
git checkout main
git pull                                  # 看看别人有没有更新
git merge dev/member-b

# 推送
git push
```

---

## 八、常见禁忌

| 序号 | 禁忌                              | 错误示例                                | 后果                                                                            | 正确做法                                                                              |
| ---- | --------------------------------- | --------------------------------------- | ------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------- |
| 1    | 头文件中 `using namespace std;` | `using namespace std;` 写在 `.h` 里 | 所有 include 该文件的代码都被强制引入 std 命名空间，造成符号污染、命名冲突      | 头文件内只用 `std::string` 等完整限定名；`.cpp` 内才可写 `using namespace std;` |
| 2    | 魔数                              | `if (cityCount > 100)`                | 数字含义不明，到处散落，修改时容易漏改                                          | `#define MAX_CITY_COUNT 100` 或 `const int MAX_CITY_COUNT = 100;`                 |
| 3    | 无意义变量名                      | `int a, b, tmp, x1, foo;`             | 读代码的人看不懂含义，答辩时老师问"这个 a 是什么"                               | `int city_count, edge_weight, start_index;`                                         |
| 4    | `gets()`                        | `gets(buffer);`                       | `gets()` 不检查缓冲区长度，输入过长直接内存越界崩溃，C11 标准已移除该函数     | `std::cin.getline(buffer, sizeof(buffer));`                                         |
| 5    | 大数组                            | `void func() { int arr[1000000]; }`   | 栈空间默认 1~8MB，大数组直接栈溢出导致程序崩溃（无任何错误提示）                | `int* arr = new (std::nothrow) int[1000000];` 用完 `delete[] arr; arr = nullptr;` |
| 6    | 悬空指针                          | `delete ptr;` 后继续使用 `ptr`      | `delete` 后内存已归还系统，继续访问 → 未定义行为（可能崩溃、可能读到脏数据） | `delete ptr; ptr = nullptr;` 之后使用前检查 `if (ptr != nullptr)`                 |
