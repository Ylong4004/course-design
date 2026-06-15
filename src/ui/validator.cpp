/*
 * 模块名称  : UI——输入校验
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 用户输入合法性校验——拦截非法字符、越界数值、重复操作等。
 */

#include "validator.h"

#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <limits>

namespace {

std::string trim_copy(const std::string &text) {
    const std::string whitespace = " \t\n\r\f\v";
    const std::size_t begin = text.find_first_not_of(whitespace);
    if (begin == std::string::npos) {
        return "";
    }

    const std::size_t end = text.find_last_not_of(whitespace);
    return text.substr(begin, end - begin + 1);
}

bool has_forbidden_windows_char(char ch) {
    switch (ch) {
    case '<':
    case '>':
    case ':':
    case '"':
    case '|':
    case '?':
    case '*':
        return true;
    default:
        return false;
    }
}

} // namespace

bool Validator::is_valid_int(const std::string &input) {
    const std::string trimmed = trim_copy(input);
    if (trimmed.empty()) {
        return false;
    }

    errno = 0;
    char *end_ptr = nullptr;
    const long value = std::strtol(trimmed.c_str(), &end_ptr, 10);
    if (errno == ERANGE || end_ptr == trimmed.c_str() || *end_ptr != '\0') {
        return false;
    }

    return value >= std::numeric_limits<int>::min() &&
           value <= std::numeric_limits<int>::max();
}

bool Validator::is_in_range(int value, int min_val, int max_val,
                            const char *field_name) {
    if (value < min_val || value > max_val) {
        std::cerr << "[错误] " << (field_name == nullptr ? "数值" : field_name)
                  << " 超出范围，应在 [" << min_val << ", " << max_val
                  << "] 之间。" << std::endl;
        return false;
    }

    return true;
}

bool Validator::is_positive_int(int value, const char *field_name) {
    if (value <= 0) {
        std::cerr << "[错误] " << (field_name == nullptr ? "数值" : field_name)
                  << " 必须是正整数。" << std::endl;
        return false;
    }

    return true;
}

bool Validator::is_valid_city_name(const std::string &name) {
    const std::string trimmed = trim_copy(name);
    if (trimmed.empty() || trimmed.size() >= MAX_CITY_NAME) {
        return false;
    }

    for (unsigned char ch : trimmed) {
        if (std::iscntrl(ch)) {
            return false;
        }
    }

    return true;
}

bool Validator::check_length(const std::string &str,
                             int min_len, int max_len,
                             const char *field_name) {
    const int length = static_cast<int>(str.size());
    if (length < min_len || length > max_len) {
        std::cerr << "[错误] " << (field_name == nullptr ? "字符串" : field_name)
                  << " 长度应在 [" << min_len << ", " << max_len
                  << "] 之间。" << std::endl;
        return false;
    }

    return true;
}

bool Validator::validate_city_id(int city_id) {
    return is_in_range(city_id, 1, MAX_CITY_COUNT, "城市编号");
}

bool Validator::validate_weight(int weight) {
    if (weight <= 0 || weight >= INF_WEIGHT) {
        std::cerr << "[错误] 道路权值必须是正整数且小于无穷大。"
                  << std::endl;
        return false;
    }

    return true;
}

bool Validator::validate_no_self_loop(int from, int to) {
    if (from == to) {
        std::cerr << "[错误] 起点和终点不能相同，自环边不合法。"
                  << std::endl;
        return false;
    }

    return true;
}

bool Validator::is_valid_file_path(const std::string &path) {
    const std::string trimmed = trim_copy(path);
    if (trimmed.empty() || trimmed.size() > 260) {
        return false;
    }

    for (char ch : trimmed) {
        if (has_forbidden_windows_char(ch)) {
            return false;
        }
    }

    const std::size_t dot_pos = trimmed.find_last_of('.');
    if (dot_pos == std::string::npos || dot_pos + 1 >= trimmed.size()) {
        return false;
    }

    std::string suffix = trimmed.substr(dot_pos + 1);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return suffix == "txt";
}

int Validator::read_int_safe(const char *prompt,
                             int min_val, int max_val) {
    while (true) {
        if (prompt != nullptr) {
            std::cout << prompt;
        }

        std::string input;
        std::getline(std::cin, input);
        if (!is_valid_int(input)) {
            std::cerr << "[错误] 请输入合法整数。" << std::endl;
            continue;
        }

        const std::string trimmed = trim_copy(input);
        const long value = std::strtol(trimmed.c_str(), nullptr, 10);
        if (value < min_val || value > max_val) {
            std::cerr << "[错误] 输入超出范围，请输入 [" << min_val << ", "
                      << max_val << "] 之间的整数。" << std::endl;
            continue;
        }

        return static_cast<int>(value);
    }
}

void Validator::read_str_safe(const char *prompt,
                              std::string &buffer,
                              int max_len) {
    while (true) {
        if (prompt != nullptr) {
            std::cout << prompt;
        }

        std::getline(std::cin, buffer);
        const std::string trimmed = trim_copy(buffer);
        if (trimmed.empty() || static_cast<int>(trimmed.size()) > max_len) {
            std::cerr << "[错误] 字符串不能为空，且长度不能超过 "
                      << max_len << "。" << std::endl;
            continue;
        }

        buffer = trimmed;
        return;
    }
}

bool Validator::read_confirm(const char *prompt) {
    while (true) {
        if (prompt != nullptr) {
            std::cout << prompt;
        }

        std::string input;
        std::getline(std::cin, input);
        const std::string trimmed = trim_copy(input);
        if (trimmed == "y" || trimmed == "Y") {
            return true;
        }

        if (trimmed == "n" || trimmed == "N") {
            return false;
        }

        std::cerr << "[错误] 请输入 Y 或 N。" << std::endl;
    }
}