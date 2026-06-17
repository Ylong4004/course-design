/*
 * 模块名称  : CLI——命令行模式入口实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 命令行模式的全流程实现——服务初始化、模式分派、
 *             菜单/CLI交互/批处理三种模式的调度逻辑。
 */

#include "cli_app.h"
#include "command_parser.h"

#include "../ui/menu.h"
#include "../services/road_network.h"
#include "../services/congestion.h"
#include "../services/comparator.h"
#include "../services/file_io.h"
#include "../common/types.h"
#include "../common/defines.h"

#include <iostream>
#include <cstring>
#include <cstdlib>

/* ============================================================ */
/*  全局服务管理                                                */
/* ============================================================ */

/** @brief 初始化全局路网、模拟器、对比器 */
static void cli_init_services(RoadNetwork **out_network,
                              CongestionSimulator **out_simulator,
                              StructureComparator **out_comparator)
{
    safe_new(*out_network, RoadNetwork, MAX_CITY_COUNT, GRAPH_UNDIRECTED);

    GraphBase *list_graph   = (*out_network)->get_graph(STORAGE_LIST);
    GraphBase *matrix_graph = (*out_network)->get_graph(STORAGE_MATRIX);

    safe_new(*out_simulator, CongestionSimulator, matrix_graph, 100);
    safe_new(*out_comparator, StructureComparator,
             matrix_graph, list_graph);

    /* 尝试自动加载历史数据 */
    if (FileManager::data_file_exists()) {
        FileManager::auto_load(list_graph);
        FileManager::auto_load(matrix_graph);
    }
}

/** @brief 释放全局服务 */
static void cli_free_services(RoadNetwork *network,
                              CongestionSimulator *simulator,
                              StructureComparator *comparator)
{
    safe_delete(comparator);
    safe_delete(simulator);
    safe_delete(network);
}

/* ============================================================ */
/*  三种运行模式                                                */
/* ============================================================ */

/**
 * @brief 菜单模式（传统交互模式），支持内部切换到 CLI
 * @note  MenuSystem 自行管理其内部的 network/simulator/comparator，
 *        用户选择切 CLI 时用 MenuSystem 暴露的对象启动 CLI。
 */
static void cli_run_menu()
{
    /* 菜单 ↔ 命令行 循环切换，直到用户在菜单中选"退出系统" */
    while (true) {
        MenuSystem menu;
        menu.show_welcome();
        menu.run();

        /* 用户从菜单中选择"命令行模式" → 切到 CLI */
        if (menu.requested_cli_switch()) {
            CommandParser::run_interactive(*menu.get_network(),
                                           menu.get_simulator(),
                                           menu.get_comparator());
            /* CLI 中键入 menu 回到这里，继续循环 */
            continue;
        }
        /* 用户在菜单中选了"退出系统" → 真正退出 */
        break;
    }
}

/**
 * @brief CLI 交互模式——循环读取命令
 */
static void cli_run_interactive(RoadNetwork *network,
                                CongestionSimulator *simulator,
                                StructureComparator *comparator)
{
    CommandParser::run_interactive(*network, simulator, comparator);
}

/**
 * @brief 批处理模式——执行单条命令后退出
 */
static void cli_run_batch(int argc, char **argv,
                          RoadNetwork *network,
                          CongestionSimulator *simulator,
                          StructureComparator *comparator)
{
    /* 将 argv[1..] 拼接为一条命令字符串 */
    std::string cmd_line;
    for (int i = 1; i < argc; ++i) {
        if (i > 1) cmd_line += ' ';
        cmd_line += argv[i];
    }
    CommandParser::execute(cmd_line, *network, simulator, comparator);
}

/** @brief 打印使用帮助 */
static void cli_print_usage(const char *prog)
{
    std::cout << "用法:\n"
              << "  " << prog << "              启动菜单模式（默认）\n"
              << "  " << prog << " --cli       启动命令行交互模式\n"
              << "  " << prog << " -c          启动命令行交互模式\n"
              << "  " << prog << " <command>   执行单条命令后退出\n"
              << "\n示例:\n"
              << "  " << prog << " new_city 1 北京\n"
              << "  " << prog << " show_network\n"
              << "  " << prog << " dijkstra 0 5\n"
              << "  " << prog << " help\n";
}

/* ============================================================ */
/*  总入口                                                      */
/* ============================================================ */

void cli_run(int argc, char **argv)
{
    /* ---------- 帮助参数 ---------- */
    if (argc >= 2 && (std::strcmp(argv[1], "--help") == 0 ||
                      std::strcmp(argv[1], "-h") == 0)) {
        cli_print_usage(argv[0]);
        return;
    }

    /* ---------- 模式分派 ---------- */
    if (argc == 1) {
        /* 无参数 → 菜单模式（内部自行管理资源，支持切换CLI） */
        cli_run_menu();
    }
    else {
        /* --cli / -c / 批处理 → 独立创建资源，自动加载默认数据 */
        RoadNetwork *network = nullptr;
        CongestionSimulator *simulator = nullptr;
        StructureComparator *comparator = nullptr;
        cli_init_services(&network, &simulator, &comparator);

        if (std::strcmp(argv[1], "--cli") == 0 ||
            std::strcmp(argv[1], "-c") == 0) {
            cli_run_interactive(network, simulator, comparator);
        }
        else {
            cli_run_batch(argc, argv, network, simulator, comparator);
        }

        /* 退出前自动保存 */
        if (network != nullptr) {
            GraphBase *g = network->get_graph(STORAGE_MATRIX);
            if (g != nullptr && g->get_vertex_count() > 0) {
                FileManager::save_to_file(g, "../data/default.txt");
            }
        }
        cli_free_services(network, simulator, comparator);
    }
}
