/*
 * 模块名称  : UI——输出格式化
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 提供统一的表格、分隔线、对齐输出等格式化打印函数。
 */

#include "formatter.h"

#include <cstdlib>
#include <cstring>
#include <limits>

namespace {

int safe_strlen(const char *text) {
    return text == nullptr ? 0 : static_cast<int>(std::strlen(text));
}

} // namespace

void Formatter::print_line(char ch, int width) {
    if (width <= 0) {
        width = 60;
    }

    for (int i = 0; i < width; ++i) {
        std::cout << ch;
    }
    std::cout << std::endl;
}

void Formatter::print_title(const char *title) {
    const int width = 60;
    const int title_len = safe_strlen(title);
    const int inner_width = width - 2;
    int left_padding = (inner_width - title_len) / 2;
    int right_padding = inner_width - title_len - left_padding;
    if (left_padding < 0) {
        left_padding = 0;
    }
    if (right_padding < 0) {
        right_padding = 0;
    }

    print_line('=', width);
    std::cout << "|";
    for (int i = 0; i < left_padding; ++i) {
        std::cout << ' ';
    }
    std::cout << (title == nullptr ? "" : title);
    for (int i = 0; i < right_padding; ++i) {
        std::cout << ' ';
    }
    std::cout << "|" << std::endl;
    print_line('=', width);
}

void Formatter::print_sub_title(const char *subtitle) {
    print_line('-', 60);
    std::cout << (subtitle == nullptr ? "" : subtitle) << std::endl;
    print_line('-', 60);
}

void Formatter::print_table_row(const char **columns,
                                const int *widths,
                                int col_count) {
    if (columns == nullptr || widths == nullptr || col_count <= 0) {
        return;
    }

    std::cout << "|";
    for (int i = 0; i < col_count; ++i) {
        const char *cell = columns[i] == nullptr ? "" : columns[i];
        std::cout << std::left << std::setw(widths[i]) << cell << "|";
    }
    std::cout << std::endl;
}

void Formatter::print_table_sep(const int *widths, int col_count) {
    if (widths == nullptr || col_count <= 0) {
        return;
    }

    std::cout << "+";
    for (int i = 0; i < col_count; ++i) {
        for (int j = 0; j < widths[i]; ++j) {
            std::cout << '-';
        }
        std::cout << "+";
    }
    std::cout << std::endl;
}

void Formatter::print_city_path(const char **city_names,
                                const int *path,
                                int path_len) {
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

void Formatter::print_distance(int distance, const char *unit) {
    if (distance >= INF_WEIGHT) {
        std::cout << "INF";
    } else {
        std::cout << distance;
    }

    if (unit != nullptr && unit[0] != '\0') {
        std::cout << ' ' << unit;
    }
}

void Formatter::print_info(const char *msg) {
    std::cout << "[信息] " << (msg == nullptr ? "" : msg) << std::endl;
}

void Formatter::print_warning(const char *msg) {
    std::cout << "[警告] " << (msg == nullptr ? "" : msg) << std::endl;
}

void Formatter::print_success(const char *msg) {
    std::cout << "[成功] " << (msg == nullptr ? "" : msg) << std::endl;
}

void Formatter::clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

void Formatter::pause() {
    std::cout << "\n按回车键继续...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}