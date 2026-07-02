/*
 * 模块名称  : 公共——全局类型定义
 * 编写人    : 组员A（架构负责人）
 * 功能描述  : 定义全项目共用的结构体、枚举、常量
 */

#ifndef TYPES_H
#define TYPES_H

#include <iostream>
#include <cstring>
#include <climits>

/* ========================== 宏定义 ========================== */

#define MAX_CITY_NAME 32   /* 城市名称最大长度 */
#define MAX_CITY_COUNT 100 /* 最大城市数量 */
#define INF_WEIGHT INT_MAX /* 无穷大权值（表示不连通） */
#define INVALID_ID -1      /* 无效编号 */

/* ========================== 错误码 ========================== */

enum ErrorCode
{
    SUCCESS = 0,             /* 操作成功 */
    ERR_CITY_NOT_FOUND = -1, /* 城市不存在 */
    ERR_CITY_DUPLICATE = -2, /* 城市重复 */
    ERR_ROAD_EXISTS = -3,    /* 道路已存在 */
    ERR_ROAD_NOT_FOUND = -4, /* 道路不存在 */
    ERR_INVALID_INPUT = -5,  /* 非法输入 */
    ERR_FILE_OPEN_FAIL = -6, /* 文件打开失败 */
    ERR_FILE_FORMAT = -7,    /* 文件格式错误 */
    ERR_OUT_OF_MEMORY = -8,  /* 内存分配失败 */
    ERR_GRAPH_EMPTY = -9,    /* 图为空（无城市） */
    ERR_GRAPH_FULL = -10,    /* 城市数量已满 */
    ERR_HAS_CYCLE = -11,     /* 图中存在环路 */
    ERR_DISCONNECTED = -12,  /* 图不连通 */
    ERR_SELF_LOOP = -13,     /* 自环边 */
};

/* ========================== 枚举类型 ========================== */

/** @brief 图存储类型 */
enum StorageType
{
    STORAGE_MATRIX = 0, /* 邻接矩阵 */
    STORAGE_LIST = 1,   /* 邻接表 */
};

/** @brief 图的方向类型 */
enum GraphType
{
    GRAPH_UNDIRECTED = 0, /* 无向图 */
    GRAPH_DIRECTED = 1,   /* 有向图 */
};

/** @brief 算法返回值状态 */
enum AlgoStatus
{
    ALGO_SUCCESS = 0,   /* 算法执行成功 */
    ALGO_NO_PATH = 1,   /* 无可达路径 */
    ALGO_NEG_CYCLE = 2, /* 存在负权环 */
};

/* ========================== 结构体定义 ========================== */

/**
 * @brief 城市顶点
 */
typedef struct
{
    int id;                   /* 城市唯一编号 */
    char name[MAX_CITY_NAME]; /* 城市名称 */
} City_t;

/**
 * @brief 道路边（用于邻接表结点和边集合）
 */
typedef struct
{
    int from;   /* 起点城市编号 */
    int to;     /* 终点城市编号 */
    int weight; /* 权值（距离/通行时间） */
} Edge_t;

/**
 * @brief 邻接表中的边结点（链表结点）
 */
typedef struct EdgeNode
{
    int dest_city;          /* 目标城市编号 */
    int weight;             /* 边权值 */
    struct EdgeNode *next; /* 下一邻接边 */
} EdgeNode_t;

/**
 * @brief 邻接表中的顶点头结点
 */
typedef struct
{
    City_t city;            /* 城市信息 */
    EdgeNode_t *first_edge; /* 第一条邻接边 */
} AdjListHead_t;

/**
 * @brief 最短路径结果
 */
typedef struct
{
    int distance; /* 最短距离 */
    int *path;  /* 路径顶点序列（动态数组） */
    int path_len; /* 路径长度（顶点个数） */
} PathResult_t;

/**
 * @brief 性能统计指标
 */
typedef struct
{
    size_t memory_bytes;     /* 内存占用（字节） */
    double traverse_time_ms; /* 遍历耗时（毫秒） */
    int find_comparisons;    /* 查找比较次数 */
    int edge_query_count;    /* 边查询次数 */
} PerfStats_t;

/**
 * @brief 最小生成树结果
 */
typedef struct
{
    Edge_t *edges;  /* 生成树的边集合 */
    int edge_count;   /* 边的数量 */
    int total_cost;   /* 总造价/总权值 */
} MSTResult_t;

#endif /* TYPES_H */
