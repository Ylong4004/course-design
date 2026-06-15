/*
 * 模块名称  : UI——输出格式化
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 提供统一的表格、分隔线、对齐输出等格式化打印函数。
 */

#ifndef FORMATTER_H
#define FORMATTER_H

#include <iostream>
#include <iomanip>

/**
 * @class Formatter
 * @brief 输出格式化工具类
 *
 * 所有屏幕输出统一经过此类方法，保证界面风格一致。
 */
class Formatter {
public:
    /* ========== 分隔线与标题 ========== */

    /** @brief 打印分隔线 */
    static void print_line(char ch, int width);

    /** @brief 打印带标题的分隔线框 */
    static void print_title(const char* title);

    /** @brief 打印小节标题 */
    static void print_sub_title(const char* subtitle);

    /* ========== 表格输出 ========== */

    /** @brief 打印表格行（自动对齐列宽） */
    static void print_table_row(const char** columns,
                                const int* widths,
                                int col_count);

    /** @brief 打印表格分隔线 */
    static void print_table_sep(const int* widths, int col_count);

    /* ========== 数据格式化 ========== */

    /** @brief 格式化城市路径序列输出 */
    static void print_city_path(const char** city_names,
                                const int* path,
                                int path_len);

    /** @brief 格式化距离 + 时间输出 */
    static void print_distance(int distance, const char* unit);

    /* ========== 提示信息 ========== */

    static void print_info(const char* msg);
    static void print_warning(const char* msg);

    /** @brief 错误信息输出到 cerr（标准错误流） */
    static void print_error(const char* msg)
    {
        std::cerr << "[错误] " << msg << std::endl;
    }

    static void print_success(const char* msg);

    /* ========== 屏幕控制 ========== */

    static void clear_screen();
    static void pause();
};

#endif /* FORMATTER_H */
