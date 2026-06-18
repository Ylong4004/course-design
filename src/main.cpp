/*
 * 模块名称  : 程序入口
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 程序主入口——初始化系统，分派到菜单/CLI/Qt/批处理模式。
 *             所有 CLI 逻辑已抽离至 src/cli/ 目录。
 *             Qt GUI 逻辑已抽离至 src/qt/ 目录。
 */

#include <cstdlib>
#include <cstring>
#include "cli/cli_app.h"
#include "test/test_cases.h"

/* Qt GUI 入口（在 src/qt/qt_app.cpp 中定义，仅 Qt 版本可用） */
#ifdef QT_GUI_ENABLED
int qt_run(int argc, char **argv);
#endif

/**
 * @brief  程序主入口
 * @note   支持四种启动方式：
 *         1) 无参数      → 菜单模式（默认）
 *         2) --cli / -c  → 命令行交互模式
 *         3) --gui / -g  → Qt 可视化界面模式
 *         4) 命令+参数   → 单条命令批处理模式（执行完即退出）
 */
int main(int argc, char **argv)
{
    /* Windows 终端默认 GBK 编码，源码是 UTF-8，切换代码页避免中文乱码 */
    std::system("chcp 65001 > nul");

    //跑测试：去掉下面一行注释即可
    //run_all_tests(); return 0;

    /* Qt GUI 模式 */
#ifdef QT_GUI_ENABLED
    if (argc >= 2 && (std::strcmp(argv[1], "--gui") == 0 ||
                      std::strcmp(argv[1], "-g") == 0)) {
        return qt_run(argc, argv);
    }
#else
    if (argc >= 2 && (std::strcmp(argv[1], "--gui") == 0 ||
                      std::strcmp(argv[1], "-g") == 0)) {
        std::cerr << "[错误] 此版本未编译 Qt GUI 支持，请使用控制台模式。" << std::endl;
        return 1;
    }
#endif

    /* 菜单 / CLI / 批处理 模式 */
    cli_run(argc, argv);

    return 0;
}
