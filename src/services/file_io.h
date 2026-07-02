/*
 * 模块名称  : 服务——文件读写与数据持久化
 * 编写人    : 组员A（架构负责人，基础接口） + 组员C（异常完善）
 * 功能描述  : 路网数据的 JSON 文件导入、保存、开机自动加载。
 */

#ifndef FILE_IO_H
#define FILE_IO_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../graph/graph_base.h"
#include "../common/types.h"

/**
 * @class FileManager
 * @brief 路网数据文件管理器
 *
 * 文件格式（JSON）：
 *   graph_type: 0=无向图，1=有向图
 *   cities: 城市数组
 *   roads: 道路数组
 */
class FileManager {
private:
    static char default_path[256];  /* 默认文件路径缓存 */

public:
    FileManager()  {}
    ~FileManager() {}

    /**
     * @brief  将路网数据保存到 JSON 文件
     * @param  graph     图存储对象
     * @param  filepath  文件路径（nullptr 使用默认路径）
     * @return 成功返回 SUCCESS
     */
    static int save_to_file(const GraphBase* graph,
                            const char* filepath);

    /**
     * @brief  从 JSON 文件加载路网数据
     * @param  graph     图存储对象（已初始化）
     * @param  filepath  文件路径（nullptr 使用默认路径）
     * @return 成功返回 SUCCESS
     */
    static int load_from_file(GraphBase* graph,
                              const char* filepath);

    static int detect_graph_type(const char* filepath,
                                 GraphType* out_type);

    /**
     * @brief  程序启动时自动加载（调用 load_from_file 默认路径）
     * @param  graph  图存储对象
     * @return 成功返回 SUCCESS，文件不存在返回 ERR_FILE_OPEN_FAIL
     */
    static int auto_load(GraphBase* graph);

    /**
     * @brief  检测默认数据文件是否存在
     */
    static bool data_file_exists();

    /**
     * @brief 列出 data 目录下的 .json 路网文件
     * @param out_files 输出文件名或完整路径
     * @param with_path true 返回带 data 目录的路径，false 仅返回文件名
     */
    static int list_data_files(std::vector<std::string> &out_files,
                               bool with_path);

    /**
     * @brief  设置默认保存文件路径
     * @note   可在程序启动时通过菜单修改
     */
    static void set_default_path(const char* path);
};

#endif /* FILE_IO_H */
