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

/*
 * Qt GUI 入口声明（实现在 src/qt/qt_app.cpp）。
 * QT_GUI_ENABLED 由 CMake 在编译 GUI 版时定义（target_compile_definitions），
 * 编译纯控制台版时不定义。同一份 main.cpp 产出两种行为
 */
#ifdef QT_GUI_ENABLED
int qt_run(int argc, char **argv);
#endif

/**
 * @brief  程序主入口
 * @note   支持四种启动方式：
 *         1) 无参数      → 菜单模式（默认）
 *         2) --cli / -c  → 命令行交互模式
 *         3) --gui / -g  → Qt 可视化界面模式（仅 GUI 版本有效）
 *         4) 命令+参数   → 单条命令批处理模式（执行完即退出）
 */
int main(int argc, char **argv)
{
    /* Windows 终端默认 GBK 编码，源码是 UTF-8，切换代码页避免中文乱码 */
    std::system("chcp 65001 > nul");

    //跑测试：去掉下面一行注释即可
    //run_all_tests(); return 0;

    /* Qt GUI 模式：编译时定义了 QT_GUI_ENABLED 才真正启动窗口；
       否则输出友好提示，不会崩溃 */
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

    /*旧的menu.run()语句已迁移到cli_run_menu()函数中，由cli_run()函数调用*/
    cli_run(argc, argv);

    return 0;
}
