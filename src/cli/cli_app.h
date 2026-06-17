/*
 * 模块名称  : CLI——命令行模式入口
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 命令行模式的总入口——模式分派、全局服务初始化、
 *             菜单/CLI交互/批处理三种模式的统一调度。
 *             从主函数中分离，遵循编码规范的模块化原则。
 */

#ifndef CLI_APP_H
#define CLI_APP_H

/**
 * @brief  命令行模式总入口
 * @param  argc  命令行参数个数
 * @param  argv  命令行参数数组
 * @note   三种启动方式：
 *         1) 无参数      → 菜单模式（支持内部切CLI）
 *         2) --cli / -c  → 命令行交互模式
 *         3) 命令+参数   → 单条命令批处理（执行完即退出）
 */
void cli_run(int argc, char **argv);

#endif /* CLI_APP_H */
