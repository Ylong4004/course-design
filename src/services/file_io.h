/*
 * 模块名称  : 服务——文件读写与数据持久化
 * 编写人    : 组员A（架构负责人，基础接口） + 组员C（异常完善）
 * 功能描述  : 路网数据的 TXT 文件导入、保存、开机自动加载。
 */

#ifndef FILE_IO_H
#define FILE_IO_H

#include <fstream>
#include <iostream>
#include "../graph/graph_base.h"
#include "../common/types.h"

/**
 * @class FileManager
 * @brief 路网数据文件管理器
 *
 * 文件格式（TXT）：
 *   第一行：图类型 城市数量 道路数量
 *   后续每行一个城市：ID 名称
 *   后续每行一条道路：起点ID 终点ID 权值
 */
class FileManager {
private:
    static char default_path[256];  /* 默认文件路径缓存 */

public:
    FileManager()  {}
    ~FileManager() {}

    /**
     * @brief  将路网数据保存到 TXT 文件
     * @param  graph     图存储对象
     * @param  filepath  文件路径（nullptr 使用默认路径）
     * @return 成功返回 SUCCESS
     */
    static int save_to_file(const GraphBase* graph,
                            const char* filepath);

    /**
     * @brief  从 TXT 文件加载路网数据
     * @param  graph     图存储对象（已初始化）
     * @param  filepath  文件路径（nullptr 使用默认路径）
     * @return 成功返回 SUCCESS
     */
    static int load_from_file(GraphBase* graph,
                              const char* filepath);

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
     * @brief  设置默认保存文件路径
     * @note   可在程序启动时通过菜单修改
     */
    static void set_default_path(const char* path);
};

#endif /* FILE_IO_H */
