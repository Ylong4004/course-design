/*
 * 模块名称  : UI——输入校验
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 用户输入合法性校验——拦截非法字符、越界数值、重复操作等。
 */

#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <iostream>
#include <string>
#include <limits>

/**
 * @class Validator
 * @brief 输入校验器
 *
 * 每个校验方法返回 true 表示通过，false 表示非法。
 * 非法时自动输出错误提示到 std::cerr。
 */
class Validator {
public:
    /* ========== 字符串校验 ========== */

    /** @brief 校验是否为有效整数 */
    static bool is_valid_int(const std::string& input);

    /** @brief 校验整数是否在 [min_val, max_val] 范围内 */
    static bool is_in_range(int value, int min_val, int max_val,
                            const char* p_field_name);

    /** @brief 校验是否为正整数 */
    static bool is_positive_int(int value, const char* p_field_name);

    /* ========== 字符串校验 ========== */

    /** @brief 校验城市名称是否合法（非空、无非法字符） */
    static bool is_valid_city_name(const std::string& name);

    /** @brief 校验字符串长度 */
    static bool check_length(const std::string& str,
                             int min_len, int max_len,
                             const char* p_field_name);

    /* ========== 业务校验 ========== */

    /** @brief 校验城市编号的合法性 */
    static bool validate_city_id(int city_id);

    /** @brief 校验道路权值的合法性 */
    static bool validate_weight(int weight);

    /** @brief 校验是否自环（from == to） */
    static bool validate_no_self_loop(int from, int to);

    /** @brief 校验文件路径是否合法 */
    static bool is_valid_file_path(const std::string& path);

    /* ========== 安全输入函数 ========== */

    /** @brief 安全读取整数（带重试和错误提示） */
    static int read_int_safe(const char* p_prompt,
                             int min_val, int max_val);

    /** @brief 安全读取字符串（带长度限制） */
    static void read_str_safe(const char* p_prompt,
                              std::string& buffer,
                              int max_len);

    /** @brief 读取确认（Y/N） */
    static bool read_confirm(const char* p_prompt);
};

#endif /* VALIDATOR_H */
