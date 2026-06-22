# CLI 命令速查手册

> 模块：`src/cli/command_parser.h` / `.cpp`
> 编写人：组员C（交互&测试负责人）
> 用法：程序启动后输入 `help` 即可查看精简版

---

## 启动方式

```bash
network_console.exe                        # 菜单模式（默认）
network_console.exe --cli                  # CLI 交互模式
network_console.exe <命令> <参数...>        # 单条命令批处理
```

---

## 命令清单（共 21 条）

### 一、路网编辑

| 命令             | 参数                             | 说明                         | 示例                   |
| ---------------- | -------------------------------- | ---------------------------- | ---------------------- |
| `new_city`     | `<id>` `<name>`              | 添加城市                     | `new_city 1 北京`    |
| `del_city`     | `<id>`                         | 删除城市（级联删除关联道路） | `del_city 1`         |
| `new_road`     | `<from>` `<to>` `<weight>` | 添加道路                     | `new_road 1 2 50`    |
| `del_road`     | `<from>` `<to>`              | 删除道路                     | `del_road 1 2`       |
| `update_road`  | `<from>` `<to>` `<weight>` | 修改道路权值                 | `update_road 1 2 80` |
| `show_network` | —                               | 打印路网详细信息             | `show_network`       |

### 二、图遍历

| 命令    | 参数           | 说明         | 示例      |
| ------- | -------------- | ------------ | --------- |
| `dfs` | `<start_id>` | 深度优先遍历 | `dfs 0` |
| `bfs` | `<start_id>` | 广度优先遍历 | `bfs 0` |

### 三、最短路径

| 命令         | 参数                  | 说明                     | 示例             |
| ------------ | --------------------- | ------------------------ | ---------------- |
| `dijkstra` | `<start>` `[end]` | 单源最短路径（选填终点） | `dijkstra 0 5` |
| `floyd`    | —                    | 多源最短路径全表         | `floyd`        |

### 四、最小生成树

| 命令        | 参数 | 说明               | 示例        |
| ----------- | ---- | ------------------ | ----------- |
| `prim`    | —   | Prim 最小生成树    | `prim`    |
| `kruskal` | —   | Kruskal 最小生成树 | `kruskal` |

### 五、拓扑排序

| 命令     | 参数 | 说明                             | 示例     |
| -------- | ---- | -------------------------------- | -------- |
| `topo` | —   | 拓扑排序（Kahn 算法 + 环路检测） | `topo` |

### 六、拥堵模拟

| 命令        | 参数                             | 说明                     | 示例                |
| ----------- | -------------------------------- | ------------------------ | ------------------- |
| `congest` | `<from>` `<to>` `<weight>` | 设置道路拥堵（增大权值） | `congest 1 2 999` |
| `restore` | —                               | 恢复所有拥堵道路         | `restore`         |

### 七、性能对比 & 文件管理 & 系统

| 命令        | 参数           | 说明                       | 示例                              |
| ----------- | -------------- | -------------------------- | --------------------------------- |
| `compare` | —             | 邻接矩阵 vs 邻接表性能对比 | `compare`                       |
| `save`    | `[filepath]` | 保存路网（默认路径）       | `save` / `save ./data/my.txt` |
| `load`    | `[filepath]` | 加载路网（默认路径）       | `load` / `load ./data/my.txt` |
| `help`    | —             | 显示命令帮助               | `help`                          |
| `menu`    | —             | 切回菜单模式（仅交互模式） | `menu`                          |
| `exit`    | —             | 退出程序                   | `exit`                          |

---

## 底层 API 调用关系

| 命令             | 调用的核心函数                                                              |
| ---------------- | --------------------------------------------------------------------------- |
| `new_city`     | `RoadNetwork::add_city(id, name)`                                         |
| `del_city`     | `RoadNetwork::remove_city(id)`                                            |
| `new_road`     | `RoadNetwork::add_road(from, to, weight)`                                 |
| `del_road`     | `RoadNetwork::remove_road(from, to)`                                      |
| `update_road`  | `RoadNetwork::update_road_weight(from, to, w)`                            |
| `show_network` | `RoadNetwork::print_network_detail()`                                     |
| `dfs`          | `traverse_dfs()` → `print_traversal_sequence()`                        |
| `bfs`          | `traverse_bfs()` → `print_traversal_sequence()`                        |
| `dijkstra`     | `run_dijkstra()` → `dijkstra_get_path()` / `print_dijkstra_result()` |
| `floyd`        | `run_floyd()` → `print_floyd_table()`                                  |
| `prim`         | `build_mst_prim()` → `print_mst_result()` → `free_mst_result()`     |
| `kruskal`      | `build_mst_kruskal()` → `print_mst_result()` → `free_mst_result()`  |
| `topo`         | `run_topological_sort()` → `print_topo_result()`                       |
| `congest`      | `CongestionSimulator::set_congestion(from, to, w)`                        |
| `restore`      | `CongestionSimulator::restore_all()`                                      |
| `compare`      | `StructureComparator::run_full_comparison()`                              |
| `save`         | `FileManager::save_to_file(graph, path)`                                  |
| `load`         | `FileManager::load_from_file(graph, path)`                                |

---

## 约定

- `<参数>` 必填，`[参数]` 选填
- 城市 ID 和权值均为正整数
- 城市名称最长 31 字符（`MAX_CITY_NAME - 1`）
- 道路权值范围：1 ~ `INT_MAX - 1`
- 交互模式下 `menu` 回到菜单，`exit` 退出
- 批处理模式下执行完自动退出
