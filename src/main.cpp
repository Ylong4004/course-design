/*
 * 模块名称  : 程序入口
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 程序主入口——初始化系统，分派到菜单模式或命令行模式。
 *             所有 CLI 逻辑已抽离至 src/cli/ 目录。
 */

#include <cstdlib>
#include "cli/cli_app.h"
#include "test/test_cases.h"

/**
 * @brief  程序主入口
 * @note   支持三种启动方式：
 *         1) 无参数      → 菜单模式（默认）
 *         2) --cli / -c  → 命令行交互模式
 *         3) 命令+参数   → 单条命令批处理模式（执行完即退出）
 */
int main(int argc, char **argv)
{
    /* Windows 终端默认 GBK 编码，源码是 UTF-8，切换代码页避免中文乱码 */
    std::system("chcp 65001 > nul");

    //跑测试：去掉下面一行注释即可
    //run_all_tests(); return 0;

    cli_run(argc, argv);

    return 0;
}
