/*
 * 模块名称  : CLI——命令行解析器
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 提供命令行模式（CLI），解析用户输入的命令并直接调用底层服务。
 *             保留原有菜单模式，新增命令行交互模式，支持批处理风格操作。
 */

#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "../services/road_network.h"
#include "../services/congestion.h"
#include "../services/comparator.h"
#include "../services/file_io.h"
#include "../graph/graph_base.h"

#include <string>
#include <vector>

/**
 * @class CommandParser
 * @brief 命令行解析器——命令行模式的核心控制器
 *
 * 支持的命令模式：
 *   new_city <id> <name>              — 添加城市
 *   del_city <id>                     — 删除城市
 *   new_road <from> <to> <weight>     — 添加道路
 *   del_road <from> <to>              — 删除道路
 *   update_road <from> <to> <weight>  — 修改道路权值
 *   show_network                      — 打印路网总览
 *   dfs <start_id>                    — 深度优先遍历
 *   bfs <start_id>                    — 广度优先遍历
 *   dijkstra <start_id> [end_id]      — 单源最短路径
 *   floyd                             — 多源最短路径
 *   prim                              — Prim最小生成树
 *   kruskal                           — Kruskal最小生成树
 *   topo                              — 拓扑排序
 *   congest <from> <to> <weight>      — 设置拥堵
 *   restore                           — 恢复拥堵
 *   compare                           — 结构性能对比
 *   save [filepath]                   — 保存到文件
 *   load [filepath]                   — 从文件加载
 *   list                              — 列出可用路网文件
 *   congest_list                      — 查看当前拥堵记录
 *   congest_report <start>            — 拥堵前后对比分析
 *   help                              — 显示命令帮助
 *   menu                              — 切换回菜单模式
 *   exit / quit                       — 退出程序
 */
class CommandParser
{
public:
    /**
     * @brief 解析并执行单条命令字符串
     * @param cmd_line  用户输入的完整命令行
     * @param network   路网管理器引用
     * @param simulator 拥堵模拟器引用（可为nullptr）
     * @param comparator 性能对比器引用（可为nullptr）
     * @return true  = 继续读取下一条命令
     *         false = 收到 exit/quit，通知外层结束
     */
    static bool execute(const std::string &cmd_line,
                        RoadNetwork &network,
                        CongestionSimulator *simulator,
                        StructureComparator *comparator);

    /** @brief 打印命令帮助列表 */
    static void print_help();

    /** @brief 启动交互式命令行循环 */
    static void run_interactive(RoadNetwork &network,
                                CongestionSimulator *simulator,
                                StructureComparator *comparator);

private:
    /* ---------- 命令分派 ---------- */
    static bool dispatch(const std::vector<std::string> &argv,
                         RoadNetwork &network,
                         CongestionSimulator *simulator,
                         StructureComparator *comparator);

    /* ---------- 各命令处理 ---------- */
    static void cmd_new_city(const std::vector<std::string> &argv,
                             RoadNetwork &network);
    static void cmd_del_city(const std::vector<std::string> &argv,
                             RoadNetwork &network);
    static void cmd_new_road(const std::vector<std::string> &argv,
                             RoadNetwork &network);
    static void cmd_del_road(const std::vector<std::string> &argv,
                             RoadNetwork &network);
    static void cmd_update_road(const std::vector<std::string> &argv,
                                RoadNetwork &network);
    static void cmd_show_network(const RoadNetwork &network);
    static void cmd_dfs(const std::vector<std::string> &argv,
                        RoadNetwork &network);
    static void cmd_bfs(const std::vector<std::string> &argv,
                        RoadNetwork &network);
    static void cmd_dijkstra(const std::vector<std::string> &argv,
                             RoadNetwork &network);
    static void cmd_floyd(RoadNetwork &network);
    static void cmd_prim(RoadNetwork &network);
    static void cmd_kruskal(RoadNetwork &network);
    static void cmd_topo(RoadNetwork &network);
    static void cmd_congest(const std::vector<std::string> &argv,
                            RoadNetwork &network,
                            CongestionSimulator *simulator);
    static void cmd_restore(RoadNetwork &network,
                            CongestionSimulator *simulator);
    static void cmd_compare(RoadNetwork &network,
                            StructureComparator *comparator);
    static void cmd_save(const std::vector<std::string> &argv,
                         RoadNetwork &network);
    static void cmd_load(const std::vector<std::string> &argv,
                         RoadNetwork &network);
    static void cmd_list();
    static void cmd_congest_list(CongestionSimulator *simulator);
    static void cmd_congest_report(const std::vector<std::string> &argv,
                                    CongestionSimulator *simulator);

    /* ---------- 工具函数 ---------- */
    static std::vector<std::string> tokenize(const std::string &line);
    static bool is_integer(const std::string &s);
    static int  to_int(const std::string &s);
    static void print_error(const char *msg);
    static void print_success(const char *msg);
};

#endif /* COMMAND_PARSER_H */
