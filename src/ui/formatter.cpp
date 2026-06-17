/*
 * 模块名称  : UI——输出格式化
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 提供统一的表格、分隔线、对齐输出等格式化打印函数。
 */

#include "formatter.h"

#include "../common/types.h"

#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>

static const int DEFAULT_LINE_WIDTH = 60;

/**
 * @brief 将空指针转换为空字符串，避免后续输出时崩溃。
 * @param text 输入字符串指针（可为 nullptr）
 * @return 原始字符串或空字符串 ""
 */
static const char *safe_text(const char *text)
{
    return text == nullptr ? "" : text;
}

/**
 * @brief 安全计算字符串长度，空指针视为长度为 0。
 * @param text 输入字符串指针（可为 nullptr）
 * @return 字符串长度
 */
static int safe_strlen(const char *text)
{
    return static_cast<int>(std::strlen(safe_text(text)));
}

/**
 * @brief 打印一条由指定字符组成的水平分割线。
 * @param ch 构成线条的字符
 * @param width 线条宽度（<=0 时使用默认宽度）
 */
void Formatter::print_line(char ch, int width)
{
    if (width <= 0) {
        width = DEFAULT_LINE_WIDTH;
    }

    for (int i = 0; i < width; ++i) {
        std::cout << ch;
    }
    std::cout << std::endl;
}

/**
 * @brief 打印带边框装饰的居中标题。
 * @param title 标题文本
 */
void Formatter::print_title(const char *title)
{
    const int width = DEFAULT_LINE_WIDTH;
    const int inner_width = width - 2;
    const int title_len = safe_strlen(title);

    print_line('=', width);

    if (title_len >= inner_width) {
        std::cout << "|" << safe_text(title) << "|" << std::endl;
    } else {
        const int left_padding = (inner_width - title_len) / 2;
        const int right_padding = inner_width - title_len - left_padding;

        std::cout << "|";
        for (int i = 0; i < left_padding; ++i) {
            std::cout << ' ';
        }
        std::cout << safe_text(title);
        for (int i = 0; i < right_padding; ++i) {
            std::cout << ' ';
        }
        std::cout << "|" << std::endl;
    }

    print_line('=', width);
}

/**
 * @brief 打印带横线装饰的子标题。
 * @param subtitle 子标题文本
 */
void Formatter::print_sub_title(const char *subtitle)
{
    print_line('-', DEFAULT_LINE_WIDTH);
    std::cout << safe_text(subtitle) << std::endl;
    print_line('-', DEFAULT_LINE_WIDTH);
}

/**
 * @brief 按指定列宽打印一行表格数据（左对齐，竖线分隔）。
 * @param columns 各列的文本数组
 * @param widths 各列的宽度数组
 * @param col_count 列数
 */
void Formatter::print_table_row(const char **columns,
                                const int *widths,
                                int col_count)
{
    if (columns == nullptr || widths == nullptr || col_count <= 0) {
        return;
    }

    std::cout << "|";
    for (int i = 0; i < col_count; ++i) {
        const int width = widths[i] > 0 ? widths[i] : 1;
        std::cout << std::left << std::setw(width)
                  << safe_text(columns[i]) << "|";
    }
    std::cout << std::endl;
}

/**
 * @brief 打印表格的行分隔线（由 '+' 和 '-' 组成）。
 * @param widths 各列的宽度数组
 * @param col_count 列数
 */
void Formatter::print_table_sep(const int *widths, int col_count)
{
    if (widths == nullptr || col_count <= 0) {
        return;
    }

    std::cout << "+";
    for (int i = 0; i < col_count; ++i) {
        const int width = widths[i] > 0 ? widths[i] : 1;
        for (int j = 0; j < width; ++j) {
            std::cout << '-';
        }
        std::cout << "+";
    }
    std::cout << std::endl;
}

/**
 * @brief 以 "A -> B -> C" 格式打印城市路径序列，优先使用城市名称。
 * @param city_names 城市名称映射数组（可为 nullptr）
 * @param path 路径上的城市ID数组
 * @param path_len 路径长度
 */
void Formatter::print_city_path(const char **city_names,
                                const int *path,
                                int path_len)
{
    if (path == nullptr || path_len <= 0) {
        std::cout << "[无路径]" << std::endl;
        return;
    }

    for (int i = 0; i < path_len; ++i) {
        if (i > 0) {
            std::cout << " -> ";
        }

        const int city_id = path[i];
        const char *city_name = nullptr;
        if (city_names != nullptr && city_id >= 0) {
            city_name = city_names[city_id];
        }

        if (city_name != nullptr && city_name[0] != '\0') {
            std::cout << city_name;
        } else {
            std::cout << city_id;
        }
    }

    std::cout << std::endl;
}

/**
 * @brief 打印距离值，不可达时显示 INF，可选附加单位。
 * @param distance 距离数值
 * @param unit 单位字符串（如 "km"），可为 nullptr
 */
void Formatter::print_distance(int distance, const char *unit)
{
    if (distance == INF_WEIGHT) {
        std::cout << "INF";
    } else {
        std::cout << distance;
    }

    if (unit != nullptr && unit[0] != '\0') {
        std::cout << ' ' << unit;
    }
}

/**
 * @brief 以 "[信息]" 前缀打印提示消息。
 * @param msg 消息文本
 */
void Formatter::print_info(const char *msg)
{
    std::cout << "[信息] " << safe_text(msg) << std::endl;
}

/**
 * @brief 以 "[警告]" 前缀打印警告消息。
 * @param msg 消息文本
 */
void Formatter::print_warning(const char *msg)
{
    std::cout << "[警告] " << safe_text(msg) << std::endl;
}

/**
 * @brief 以 "[成功]" 前缀打印成功消息。
 * @param msg 消息文本
 */
void Formatter::print_success(const char *msg)
{
    std::cout << "[成功] " << safe_text(msg) << std::endl;
}

/**
 * @brief 清空控制台屏幕（Windows 用 cls，其他平台用 clear）。
 */
void Formatter::clear_screen()
{
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

/**
 * @brief 暂停程序执行，提示用户按回车键继续。
 */
void Formatter::pause()
{
    std::cout << "\n按回车键继续...";
    std::cout.flush();

    std::string dummy;
    if (!std::getline(std::cin, dummy)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}
