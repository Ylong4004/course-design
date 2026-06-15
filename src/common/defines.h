/*
 * 模块名称  : 公共——宏定义与编译选项
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 全局宏定义、调试开关、系统常量
 */

#ifndef DEFINES_H
#define DEFINES_H

#include <iostream>

/* ========================== 调试开关 ========================== */

// #define DEBUG_MODE          /* 开启后输出详细调试信息 */

#ifdef DEBUG_MODE
#define DEBUG_PRINT(msg)  std::cout << "[DEBUG] " << msg << std::endl
#else
#define DEBUG_PRINT(msg)  ((void)0)
#endif

/* ========================== 安全内存宏 ========================== */

/**
 * @brief 安全分配单个对象（调用构造函数）
 * @note  失败时打印错误信息并退出
 *
 * 用法：safe_new(graph, AdjMatrix, 100, GRAPH_UNDIRECTED);
 */
#define safe_new(ptr, Type, ...)                                            \
    do                                                                      \
    {                                                                       \
        (ptr) = new (std::nothrow) Type(__VA_ARGS__);                      \
        if ((ptr) == nullptr)                                               \
        {                                                                   \
            std::cerr << "[错误] 内存分配失败: " << __FILE__                \
                      << ", 行 " << __LINE__ << std::endl;                 \
            exit(EXIT_FAILURE);                                             \
        }                                                                   \
    } while (0)

/**
 * @brief 安全分配数组
 * @note  失败时打印错误信息并退出
 *
 * 用法：safe_new_array(p_matrix, int, rows);
 */
#define safe_new_array(ptr, Type, count)                                    \
    do                                                                      \
    {                                                                       \
        (ptr) = new (std::nothrow) Type[(count)];                          \
        if ((ptr) == nullptr)                                               \
        {                                                                   \
            std::cerr << "[错误] 内存分配失败: " << __FILE__                \
                      << ", 行 " << __LINE__ << std::endl;                 \
            exit(EXIT_FAILURE);                                             \
        }                                                                   \
    } while (0)

/**
 * @brief 安全释放单个对象（调用析构函数并置空）
 *
 * 用法：safe_delete(graph);
 */
#define safe_delete(ptr) \
    do                   \
    {                    \
        delete (ptr);    \
        (ptr) = nullptr; \
    } while (0)

/**
 * @brief 安全释放数组（调用析构函数并置空）
 *
 * 用法：safe_delete_array(p_buffer);
 */
#define safe_delete_array(ptr) \
    do                         \
    {                          \
        delete[] (ptr);        \
        (ptr) = nullptr;       \
    } while (0)

/* ========================== 工具宏 ========================== */

/** @brief 取两数较小值 */
#define min_val(a, b) (((a) < (b)) ? (a) : (b))

/** @brief 取两数较大值 */
#define max_val(a, b) (((a) > (b)) ? (a) : (b))

/** @brief 交换两个变量 */
#define swap_val(type, a, b) \
    do                   \
    {                    \
        type _tmp = (a); \
        (a) = (b);       \
        (b) = _tmp;      \
    } while (0)

/** @brief 清空输入缓冲区 */
#define clear_stdin()                                   \
    do                                                  \
    {                                                   \
        while (std::cin.get() != '\n' && !std::cin.eof()) \
            ;                                           \
    } while (0)

/** @brief 按任意键继续 */
#define pause_console()                          \
    do                                    \
    {                                     \
        std::cout << "\n按 Enter 键继续..."; \
        clear_stdin();                      \
    } while (0)

#endif /* DEFINES_H */
