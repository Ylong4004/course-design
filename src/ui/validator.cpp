/*
 * 模块名称  : UI——输入校验
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 用户输入合法性校验——拦截非法字符、越界数值、重复操作等。
 */

#include "validator.h"

#include "../common/types.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdlib>

static std::string trim_copy(const std::string &text)
{
    const std::string whitespace = " \t\n\r\f\v";
    const std::size_t begin = text.find_first_not_of(whitespace);
    if (begin == std::string::npos) {
        return "";
    }

    const std::size_t end = text.find_last_not_of(whitespace);
    return text.substr(begin, end - begin + 1);
}

static bool is_decimal_digits(const std::string &text, std::size_t begin)
{
    if (begin >= text.size()) {
        return false;
    }

    for (std::size_t i = begin; i < text.size(); ++i) {
        const unsigned char ch = static_cast<unsigned char>(text[i]);
        if (!std::isdigit(ch)) {
            return false;
        }
    }

    return true;
}

static bool has_forbidden_path_char(char ch)
{
    switch (ch) {
    case '<':
    case '>':
    case '"':
    case '|':
    case '?':
    case '*':
        return true;
    default:
        return false;
    }
}

static bool has_txt_suffix(const std::string &path)
{
    const std::size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos || dot_pos + 1 >= path.size()) {
        return false;
    }

    std::string suffix = path.substr(dot_pos + 1);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(),
                   [](unsigned char ch) {
                       return static_cast<char>(std::tolower(ch));
                   });

    return suffix == "txt";
}

static std::string to_lower_copy(const std::string &text)
{
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char ch) {
                       return static_cast<char>(std::tolower(ch));
                   });
    return result;
}

bool Validator::is_valid_int(const std::string &input)
{
    const std::string trimmed = trim_copy(input);
    if (trimmed.empty()) {
        return false;
    }

    std::size_t digit_begin = 0;
    if (trimmed[0] == '+' || trimmed[0] == '-') {
        digit_begin = 1;
    }
    if (!is_decimal_digits(trimmed, digit_begin)) {
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
                            const char *field_name)
{
    if (min_val > max_val) {
        std::cerr << "[错误] 校验范围配置错误。" << std::endl;
        return false;
    }

    if (value < min_val || value > max_val) {
        std::cerr << "[错误] " << (field_name == nullptr ? "数值" : field_name)
                  << " 超出范围，应在 [" << min_val << ", " << max_val
                  << "] 之间。" << std::endl;
        return false;
    }

    return true;
}

bool Validator::is_positive_int(int value, const char *field_name)
{
    if (value <= 0) {
        std::cerr << "[错误] " << (field_name == nullptr ? "数值" : field_name)
                  << " 必须是正整数。" << std::endl;
        return false;
    }

    return true;
}

bool Validator::is_valid_city_name(const std::string &name)
{
    const std::string trimmed = trim_copy(name);
    if (trimmed.empty() || static_cast<int>(trimmed.size()) >= MAX_CITY_NAME) {
        std::cerr << "[错误] 城市名称不能为空，且长度必须小于 "
                  << MAX_CITY_NAME << " 个字节。" << std::endl;
        return false;
    }

    for (char raw_ch : trimmed) {
        const unsigned char ch = static_cast<unsigned char>(raw_ch);
        if (std::iscntrl(ch)) {
            std::cerr << "[错误] 城市名称不能包含控制字符。" << std::endl;
            return false;
        }
    }

    return true;
}

bool Validator::check_length(const std::string &str,
                             int min_len, int max_len,
                             const char *field_name)
{
    if (min_len < 0 || max_len < min_len) {
        std::cerr << "[错误] 长度校验范围配置错误。" << std::endl;
        return false;
    }

    const int length = static_cast<int>(str.size());
    if (length < min_len || length > max_len) {
        std::cerr << "[错误] " << (field_name == nullptr ? "字符串" : field_name)
                  << " 长度应在 [" << min_len << ", " << max_len
                  << "] 之间。" << std::endl;
        return false;
    }

    return true;
}

bool Validator::validate_city_id(int city_id)
{
    return is_in_range(city_id, 1, MAX_CITY_COUNT, "城市编号");
}

bool Validator::validate_weight(int weight)
{
    if (weight <= 0 || weight >= INF_WEIGHT) {
        std::cerr << "[错误] 道路权值必须是正整数且小于无穷大。"
                  << std::endl;
        return false;
    }

    return true;
}

bool Validator::validate_no_self_loop(int from, int to)
{
    if (from == to) {
        std::cerr << "[错误] 起点和终点不能相同，自环边不合法。"
                  << std::endl;
        return false;
    }

    return true;
}

bool Validator::is_valid_file_path(const std::string &path)
{
    const std::string trimmed = trim_copy(path);
    if (trimmed.empty() || trimmed.size() > 260 || !has_txt_suffix(trimmed)) {
        return false;
    }

    for (std::size_t i = 0; i < trimmed.size(); ++i) {
        const char ch = trimmed[i];
        const unsigned char uch = static_cast<unsigned char>(ch);

        if (std::iscntrl(uch) || has_forbidden_path_char(ch)) {
            return false;
        }

        if (ch == ':') {
            const bool is_windows_drive =
                i == 1 &&
                std::isalpha(static_cast<unsigned char>(trimmed[0])) &&
                trimmed.size() >= 3 &&
                (trimmed[2] == '\\' || trimmed[2] == '/');
            if (!is_windows_drive) {
                return false;
            }
        }
    }

    const char last = trimmed[trimmed.size() - 1];
    return last != '\\' && last != '/';
}

int Validator::read_int_safe(const char *prompt,
                             int min_val, int max_val)
{
    while (true) {
        if (prompt != nullptr) {
            std::cout << prompt;
        }

        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cin.clear();
            std::cerr << "[错误] 读取输入失败，请重新输入。" << std::endl;
            continue;
        }

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
                              int max_len)
{
    while (true) {
        if (prompt != nullptr) {
            std::cout << prompt;
        }

        if (!std::getline(std::cin, buffer)) {
            std::cin.clear();
            std::cerr << "[错误] 读取输入失败，请重新输入。" << std::endl;
            continue;
        }

        buffer = trim_copy(buffer);
        if (buffer.empty()) {
            std::cerr << "[错误] 输入不能为空。" << std::endl;
            continue;
        }

        if (max_len > 0 && static_cast<int>(buffer.size()) > max_len) {
            std::cerr << "[错误] 输入长度不能超过 " << max_len
                      << " 个字节。" << std::endl;
            continue;
        }

        return;
    }
}

bool Validator::read_confirm(const char *prompt)
{
    while (true) {
        if (prompt != nullptr) {
            std::cout << prompt;
        }

        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cin.clear();
            std::cerr << "[错误] 读取输入失败，请重新输入。" << std::endl;
            continue;
        }

        const std::string trimmed = trim_copy(input);
        const std::string lower = to_lower_copy(trimmed);
        if (lower == "y" || lower == "yes" || trimmed == "是") {
            return true;
        }

        if (lower == "n" || lower == "no" || trimmed == "否") {
            return false;
        }

        std::cerr << "[错误] 请输入 Y 或 N。" << std::endl;
    }
}
