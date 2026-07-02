/*
 * 模块名称  : 服务——文件读写与数据持久化
 * 编写人    : 组员A（架构负责人，基础接口） + 组员C（异常完善）
 * 功能描述  : 路网数据的 JSON 文件导入、保存、开机自动加载。
 */

#include "file_io.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <iterator>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#endif

char FileManager::default_path[256] = "./data/default.json";

static bool path_has_separator(const std::string &path)
{
    return path.find('/') != std::string::npos ||
           path.find('\\') != std::string::npos;
}

static bool is_absolute_path(const std::string &path)
{
    if (path.empty()) {
        return false;
    }

    if (path[0] == '/' || path[0] == '\\') {
        return true;
    }

    return path.size() >= 2 && path[1] == ':';
}

static std::string get_basename(const std::string &path)
{
    std::size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return path;
    }

    return path.substr(pos + 1);
}

static bool directory_exists(const std::string &path)
{
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

static bool file_exists(const std::string &path)
{
    std::ifstream infile(path.c_str(), std::ios::in);
    return infile.is_open();
}

static bool parent_directory_exists(const std::string &path)
{
    std::size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return true;
    }

    return directory_exists(path.substr(0, pos));
}

static std::string join_path(const std::string &dir, const std::string &name)
{
    if (dir.empty()) {
        return name;
    }

    char last = dir[dir.size() - 1];
    if (last == '/' || last == '\\') {
        return dir + name;
    }

    return dir + "/" + name;
}

static bool looks_like_data_path(const std::string &path)
{
    std::string normalized = path;
    for (std::size_t i = 0; i < normalized.size(); ++i) {
        if (normalized[i] == '\\') {
            normalized[i] = '/';
        }
    }

    return !path_has_separator(path) ||
           normalized.find("data/") == 0 ||
           normalized.find("./data/") == 0 ||
           normalized.find("../data/") != std::string::npos;
}

static std::string find_data_directory()
{
    const char *candidates[] = {
        "./data",
        "../data",
        "../../data"
    };

    for (std::size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
        if (directory_exists(candidates[i])) {
            return candidates[i];
        }
    }

    return "";
}

static std::string resolve_file_path(const char *filepath, bool for_write)
{
    std::string input = (filepath != nullptr && filepath[0] != '\0')
                            ? filepath
                            : "./data/default.json";

    if (is_absolute_path(input)) {
        return input;
    }

    if (!for_write && file_exists(input)) {
        return input;
    }

    if (for_write && parent_directory_exists(input)) {
        return input;
    }

    if (!looks_like_data_path(input)) {
        return input;
    }

    std::string basename = get_basename(input);
    std::string data_dir = find_data_directory();
    if (data_dir.empty()) {
        return input;
    }

    std::string resolved = join_path(data_dir, basename);
    if (for_write || file_exists(resolved)) {
        return resolved;
    }

    return input;
}

/**
 * @brief 将 C++ 字符串中的城市名称安全拷贝到 C 风格字符数组，自动截断超长内容。
 * @param dest_name 目标字符数组指针
 * @param source_name 源 C++ 字符串
 */
static void copy_city_name(char *dest_name, const std::string &source_name)
{
    if (dest_name == nullptr)
    {
        return;
    }

    std::size_t index = 0;
    for (; index + 1 < MAX_CITY_NAME && index < source_name.size(); ++index)
    {
        dest_name[index] = source_name[index];
    }
    dest_name[index] = '\0';
}

static std::string json_escape(const char *text)
{
    std::string result;
    if (text == nullptr) {
        return result;
    }

    for (const unsigned char *p = reinterpret_cast<const unsigned char *>(text);
         *p != '\0'; ++p) {
        switch (*p) {
        case '"':
            result += "\\\"";
            break;
        case '\\':
            result += "\\\\";
            break;
        case '\b':
            result += "\\b";
            break;
        case '\f':
            result += "\\f";
            break;
        case '\n':
            result += "\\n";
            break;
        case '\r':
            result += "\\r";
            break;
        case '\t':
            result += "\\t";
            break;
        default:
            if (*p < 0x20) {
                char buffer[7];
                std::snprintf(buffer, sizeof(buffer), "\\u%04x", *p);
                result += buffer;
            } else {
                result += static_cast<char>(*p);
            }
            break;
        }
    }

    return result;
}

static void append_utf8(std::string &text, int codepoint)
{
    if (codepoint < 0) {
        return;
    }

    if (codepoint <= 0x7F) {
        text += static_cast<char>(codepoint);
    } else if (codepoint <= 0x7FF) {
        text += static_cast<char>(0xC0 | ((codepoint >> 6) & 0x1F));
        text += static_cast<char>(0x80 | (codepoint & 0x3F));
    } else {
        text += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
        text += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
        text += static_cast<char>(0x80 | (codepoint & 0x3F));
    }
}

class JsonReader
{
private:
    const std::string &text;
    std::size_t pos;

public:
    explicit JsonReader(const std::string &content)
        : text(content), pos(0)
    {
        if (text.size() >= 3 &&
            static_cast<unsigned char>(text[0]) == 0xEF &&
            static_cast<unsigned char>(text[1]) == 0xBB &&
            static_cast<unsigned char>(text[2]) == 0xBF)
        {
            pos = 3;
        }
    }

    bool parse_network(int &graph_type,
                       std::vector<City_t> &cities,
                       std::vector<Edge_t> &edges)
    {
        bool has_graph_type = false;
        bool has_cities = false;
        bool has_roads = false;

        if (!consume('{')) {
            return false;
        }

        skip_ws();
        if (consume('}')) {
            return false;
        }

        while (true) {
            std::string key;
            if (!parse_string(key) || !consume(':')) {
                return false;
            }

            if (key == "graph_type") {
                if (!parse_int(graph_type)) {
                    return false;
                }
                has_graph_type = true;
            } else if (key == "cities") {
                if (!parse_cities(cities)) {
                    return false;
                }
                has_cities = true;
            } else if (key == "roads") {
                if (!parse_roads(edges)) {
                    return false;
                }
                has_roads = true;
            } else if (!skip_value()) {
                return false;
            }

            skip_ws();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }

        skip_ws();
        return pos == text.size() && has_graph_type && has_cities && has_roads;
    }

private:
    void skip_ws()
    {
        while (pos < text.size() &&
               std::isspace(static_cast<unsigned char>(text[pos]))) {
            ++pos;
        }
    }

    bool consume(char ch)
    {
        skip_ws();
        if (pos < text.size() && text[pos] == ch) {
            ++pos;
            return true;
        }
        return false;
    }

    int parse_hex_digit(char ch) const
    {
        if (ch >= '0' && ch <= '9') {
            return ch - '0';
        }
        if (ch >= 'a' && ch <= 'f') {
            return ch - 'a' + 10;
        }
        if (ch >= 'A' && ch <= 'F') {
            return ch - 'A' + 10;
        }
        return -1;
    }

    bool parse_string(std::string &out)
    {
        skip_ws();
        if (pos >= text.size() || text[pos] != '"') {
            return false;
        }
        ++pos;

        out.clear();
        while (pos < text.size()) {
            char ch = text[pos++];
            if (ch == '"') {
                return true;
            }
            if (ch == '\\') {
                if (pos >= text.size()) {
                    return false;
                }
                char esc = text[pos++];
                switch (esc) {
                case '"':
                case '\\':
                case '/':
                    out += esc;
                    break;
                case 'b':
                    out += '\b';
                    break;
                case 'f':
                    out += '\f';
                    break;
                case 'n':
                    out += '\n';
                    break;
                case 'r':
                    out += '\r';
                    break;
                case 't':
                    out += '\t';
                    break;
                case 'u': {
                    if (pos + 4 > text.size()) {
                        return false;
                    }
                    int codepoint = 0;
                    for (int i = 0; i < 4; ++i) {
                        int digit = parse_hex_digit(text[pos++]);
                        if (digit < 0) {
                            return false;
                        }
                        codepoint = codepoint * 16 + digit;
                    }
                    append_utf8(out, codepoint);
                    break;
                }
                default:
                    return false;
                }
            } else {
                out += ch;
            }
        }

        return false;
    }

    bool parse_int(int &out)
    {
        skip_ws();
        if (pos >= text.size()) {
            return false;
        }

        bool negative = false;
        if (text[pos] == '-') {
            negative = true;
            ++pos;
        }

        if (pos >= text.size() ||
            !std::isdigit(static_cast<unsigned char>(text[pos]))) {
            return false;
        }

        long long value = 0;
        while (pos < text.size() &&
               std::isdigit(static_cast<unsigned char>(text[pos]))) {
            value = value * 10 + (text[pos] - '0');
            if (value > INT_MAX) {
                return false;
            }
            ++pos;
        }

        out = static_cast<int>(negative ? -value : value);
        return true;
    }

    bool parse_city(City_t &city)
    {
        bool has_id = false;
        bool has_name = false;
        std::string name;

        if (!consume('{')) {
            return false;
        }

        while (true) {
            std::string key;
            if (!parse_string(key) || !consume(':')) {
                return false;
            }

            if (key == "id") {
                if (!parse_int(city.id)) {
                    return false;
                }
                has_id = true;
            } else if (key == "name") {
                if (!parse_string(name)) {
                    return false;
                }
                has_name = true;
            } else if (!skip_value()) {
                return false;
            }

            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }

        if (!has_id || !has_name) {
            return false;
        }
        if (name.empty() || name.size() >= MAX_CITY_NAME) {
            return false;
        }
        copy_city_name(city.name, name);
        return true;
    }

    bool parse_edge(Edge_t &edge)
    {
        bool has_from = false;
        bool has_to = false;
        bool has_weight = false;

        if (!consume('{')) {
            return false;
        }

        while (true) {
            std::string key;
            if (!parse_string(key) || !consume(':')) {
                return false;
            }

            if (key == "from") {
                if (!parse_int(edge.from)) {
                    return false;
                }
                has_from = true;
            } else if (key == "to") {
                if (!parse_int(edge.to)) {
                    return false;
                }
                has_to = true;
            } else if (key == "weight") {
                if (!parse_int(edge.weight)) {
                    return false;
                }
                has_weight = true;
            } else if (!skip_value()) {
                return false;
            }

            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }

        return has_from && has_to && has_weight;
    }

    bool parse_cities(std::vector<City_t> &cities)
    {
        cities.clear();
        if (!consume('[')) {
            return false;
        }
        if (consume(']')) {
            return true;
        }

        while (true) {
            City_t city{};
            if (!parse_city(city)) {
                return false;
            }
            cities.push_back(city);

            if (consume(']')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return true;
    }

    bool parse_roads(std::vector<Edge_t> &edges)
    {
        edges.clear();
        if (!consume('[')) {
            return false;
        }
        if (consume(']')) {
            return true;
        }

        while (true) {
            Edge_t edge{};
            if (!parse_edge(edge)) {
                return false;
            }
            edges.push_back(edge);

            if (consume(']')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return true;
    }

    bool skip_value()
    {
        skip_ws();
        if (pos >= text.size()) {
            return false;
        }

        if (text[pos] == '"') {
            std::string ignored;
            return parse_string(ignored);
        }
        if (text[pos] == '{') {
            return skip_object();
        }
        if (text[pos] == '[') {
            return skip_array();
        }
        if (text[pos] == '-' ||
            std::isdigit(static_cast<unsigned char>(text[pos]))) {
            int ignored = 0;
            return parse_int(ignored);
        }
        if (text.compare(pos, 4, "true") == 0) {
            pos += 4;
            return true;
        }
        if (text.compare(pos, 5, "false") == 0) {
            pos += 5;
            return true;
        }
        if (text.compare(pos, 4, "null") == 0) {
            pos += 4;
            return true;
        }

        return false;
    }

    bool skip_object()
    {
        if (!consume('{')) {
            return false;
        }
        if (consume('}')) {
            return true;
        }

        while (true) {
            std::string ignored;
            if (!parse_string(ignored) || !consume(':') || !skip_value()) {
                return false;
            }
            if (consume('}')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    bool skip_array()
    {
        if (!consume('[')) {
            return false;
        }
        if (consume(']')) {
            return true;
        }

        while (true) {
            if (!skip_value()) {
                return false;
            }
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }
};

/**
 * @brief 在已解析的城市向量中线性查找指定城市ID的下标。
 * @param cities 城市向量
 * @param city_id 要查找的城市ID
 * @return 找到则返回下标，否则返回 -1
 */
static int find_city_index(const std::vector<City_t> &cities, int city_id)
{
    for (std::size_t i = 0; i < cities.size(); ++i)
    {
        if (cities[i].id == city_id)
        {
            return static_cast<int>(i);
        }
    }

    return -1;
}

/**
 * @brief 判断指定城市ID是否已在已解析的城市向量中存在。
 */
static bool city_exists(const std::vector<City_t> &cities, int city_id)
{
    return find_city_index(cities, city_id) != -1;
}

static bool same_edge(const Edge_t &left, const Edge_t &right, GraphType graph_type)
{
    int left_from = left.from;
    int left_to = left.to;
    int right_from = right.from;
    int right_to = right.to;

    if (graph_type == GRAPH_UNDIRECTED) {
        if (left_from > left_to) {
            const int temp = left_from;
            left_from = left_to;
            left_to = temp;
        }
        if (right_from > right_to) {
            const int temp = right_from;
            right_from = right_to;
            right_to = temp;
        }
    }

    return left_from == right_from && left_to == right_to;
}

/**
 * @brief 清空图中的所有顶点（进而级联删除所有边），恢复到空图状态。
 * @param graph 图存储结构指针
 * @return SUCCESS 成功，ERR_INVALID_INPUT 图指针为空
 */
static int clear_graph(GraphBase *graph)
{
    if (graph == nullptr)
    {
        return ERR_INVALID_INPUT;
    }

    int *vertex_ids = nullptr;
    int vertex_count = 0;
    int ret = graph->get_all_vertex_ids(&vertex_ids, &vertex_count);
    if (ret != SUCCESS)
    {
        return ret;
    }

    for (int i = 0; i < vertex_count; ++i)
    {
        graph->remove_vertex(vertex_ids[i]);
    }

    delete[] vertex_ids;
    return SUCCESS;
}
/* ---- 辅助函数结束 ---- */

/**
 * @brief 将图数据保存到 JSON 文件。
 * @param graph 图存储结构指针
 * @param filepath 目标文件路径，为 nullptr 时使用默认路径
 * @return SUCCESS 成功，ERR_INVALID_INPUT 图为空，ERR_FILE_OPEN_FAIL 文件打开失败
 */
static int read_network_json(const char *filepath,
                             int &file_graph_type,
                             std::vector<City_t> &cities,
                             std::vector<Edge_t> &edges)
{
    std::string path = resolve_file_path(filepath, false);
    std::ifstream infile(path.c_str(), std::ios::in);
    if (!infile.is_open())
    {
        return ERR_FILE_OPEN_FAIL;
    }

    std::string content((std::istreambuf_iterator<char>(infile)),
                        std::istreambuf_iterator<char>());

    JsonReader reader(content);
    if (!reader.parse_network(file_graph_type, cities, edges))
    {
        return ERR_FILE_FORMAT;
    }

    if (file_graph_type != GRAPH_UNDIRECTED && file_graph_type != GRAPH_DIRECTED)
    {
        return ERR_FILE_FORMAT;
    }

    return SUCCESS;
}

int FileManager::save_to_file(const GraphBase *graph,
                              const char *filepath)
{
    if (graph == nullptr)
    {
        return ERR_INVALID_INPUT;
    }

    std::string path = resolve_file_path(
        (filepath != nullptr && filepath[0] != '\0') ? filepath : default_path,
        true);
    std::ofstream outfile(path.c_str(), std::ios::out | std::ios::trunc);
    if (!outfile.is_open())
    {
        return ERR_FILE_OPEN_FAIL;
    }

    int *vertex_ids = nullptr;
    int vertex_count = 0;
    int ret = graph->get_all_vertex_ids(&vertex_ids, &vertex_count);
    if (ret != SUCCESS)
    {
        return ret;
    }

    std::vector<Edge_t> edges;
    edges.reserve(static_cast<std::size_t>(graph->get_edge_count()));

    for (int i = 0; i < vertex_count; ++i)
    {
        for (int j = 0; j < vertex_count; ++j)
        {
            if (graph->get_graph_type() == GRAPH_UNDIRECTED && i >= j)
            {
                continue;
            }

            int weight = INF_WEIGHT;
            if (graph->get_edge_weight(vertex_ids[i], vertex_ids[j], &weight) != SUCCESS)
            {
                continue;
            }

            Edge_t edge{};
            edge.from = vertex_ids[i];
            edge.to = vertex_ids[j];
            edge.weight = weight;
            edges.push_back(edge);
        }
    }

    outfile << "{\n";
    outfile << "  \"graph_type\": " << static_cast<int>(graph->get_graph_type()) << ",\n";
    outfile << "  \"cities\": [\n";

    for (int i = 0; i < vertex_count; ++i)
    {
        City_t city{};
        ret = graph->get_vertex(vertex_ids[i], &city);
        if (ret != SUCCESS)
        {
            delete[] vertex_ids;
            return ret;
        }

        outfile << "    {\"id\": " << city.id
                << ", \"name\": \"" << json_escape(city.name) << "\"}";
        if (i + 1 < vertex_count)
        {
            outfile << ',';
        }
        outfile << '\n';
    }

    outfile << "  ],\n";
    outfile << "  \"roads\": [\n";

    for (std::size_t i = 0; i < edges.size(); ++i)
    {
        outfile << "    {\"from\": " << edges[i].from
                << ", \"to\": " << edges[i].to
                << ", \"weight\": " << edges[i].weight << "}";
        if (i + 1 < edges.size())
        {
            outfile << ',';
        }
        outfile << '\n';
    }

    outfile << "  ]\n";
    outfile << "}\n";

    delete[] vertex_ids;
    return SUCCESS;
}

/**
 * @brief 从 JSON 文件加载路网数据到图中，含完整的格式校验和数据合法性检查。
 * @param graph 目标图存储结构指针（会先被清空）
 * @param filepath 源文件路径，为 nullptr 时使用默认路径
 * @return SUCCESS 成功，ERR_INVALID_INPUT 图为空，ERR_FILE_OPEN_FAIL 文件打开失败，ERR_FILE_FORMAT 文件格式错误，ERR_GRAPH_FULL 顶点数超限
 */
int FileManager::load_from_file(GraphBase *graph,
                                const char *filepath)
{
    if (graph == nullptr)
    {
        return ERR_INVALID_INPUT;
    }

    int file_graph_type = INVALID_ID;
    std::vector<City_t> cities;
    std::vector<Edge_t> edges;
    int ret = read_network_json(
        (filepath != nullptr && filepath[0] != '\0') ? filepath : default_path,
        file_graph_type,
        cities,
        edges);
    if (ret != SUCCESS)
    {
        return ret;
    }

    if (graph->get_graph_type() != static_cast<GraphType>(file_graph_type))
    {
        return ERR_FILE_FORMAT;
    }

    if (cities.size() > static_cast<std::size_t>(graph->get_max_vertex_count()))
    {
        return ERR_GRAPH_FULL;
    }

    for (std::size_t i = 0; i < cities.size(); ++i)
    {
        if (cities[i].id == INVALID_ID ||
            cities[i].name[0] == '\0' ||
            std::strlen(cities[i].name) >= MAX_CITY_NAME)
        {
            return ERR_FILE_FORMAT;
        }

        for (std::size_t j = i + 1; j < cities.size(); ++j)
        {
            if (cities[i].id == cities[j].id)
            {
                return ERR_FILE_FORMAT;
            }
        }
    }

    for (std::size_t i = 0; i < edges.size(); ++i)
    {
        if (edges[i].from == edges[i].to ||
            edges[i].weight <= 0 ||
            edges[i].weight >= INF_WEIGHT)
        {
            return ERR_FILE_FORMAT;
        }

        if (!city_exists(cities, edges[i].from) ||
            !city_exists(cities, edges[i].to))
        {
            return ERR_FILE_FORMAT;
        }

        for (std::size_t j = i + 1; j < edges.size(); ++j)
        {
            if (same_edge(edges[i], edges[j],
                          static_cast<GraphType>(file_graph_type)))
            {
                return ERR_FILE_FORMAT;
            }
        }
    }

    ret = clear_graph(graph);
    if (ret != SUCCESS)
    {
        return ret;
    }

    for (std::size_t i = 0; i < cities.size(); ++i)
    {
        ret = graph->add_vertex(cities[i]);
        if (ret != SUCCESS)
        {
            return ret;
        }
    }

    for (std::size_t i = 0; i < edges.size(); ++i)
    {
        ret = graph->add_edge(edges[i].from, edges[i].to, edges[i].weight);
        if (ret != SUCCESS)
        {
            return ret;
        }
    }

    return SUCCESS;
}

/**
 * @brief 使用默认路径自动加载路网数据，通常用于程序启动时恢复上次保存的路网。
 * @param graph 目标图存储结构指针
 * @return 同 load_from_file 的返回值
 */
int FileManager::detect_graph_type(const char *filepath,
                                   GraphType *out_type)
{
    if (out_type == nullptr)
    {
        return ERR_INVALID_INPUT;
    }

    int file_graph_type = INVALID_ID;
    std::vector<City_t> cities;
    std::vector<Edge_t> edges;
    int ret = read_network_json(
        (filepath != nullptr && filepath[0] != '\0') ? filepath : default_path,
        file_graph_type,
        cities,
        edges);
    if (ret != SUCCESS)
    {
        return ret;
    }

    *out_type = static_cast<GraphType>(file_graph_type);
    return SUCCESS;
}

int FileManager::auto_load(GraphBase *graph)
{
    return load_from_file(graph, nullptr);
}

/**
 * @brief 检查默认数据文件是否存在。
 * @return true 文件存在，false 不存在
 */
bool FileManager::data_file_exists()
{
    std::string path = resolve_file_path(default_path, false);
    std::ifstream infile(path.c_str(), std::ios::in);
    return infile.is_open();
}

/**
 * @brief 列出当前可识别 data 目录下的 .json 路网文件。
 */
int FileManager::list_data_files(std::vector<std::string> &out_files,
                                 bool with_path)
{
    out_files.clear();

    std::string data_dir = find_data_directory();
    if (data_dir.empty()) {
        return ERR_FILE_OPEN_FAIL;
    }

#ifdef _WIN32
    std::string pattern = join_path(data_dir, "*.json");
    WIN32_FIND_DATAA find_data;
    HANDLE handle = FindFirstFileA(pattern.c_str(), &find_data);
    if (handle == INVALID_HANDLE_VALUE) {
        return SUCCESS;
    }

    do {
        if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
            std::string name = find_data.cFileName;
            if (name != "_list.tmp") {
                out_files.push_back(with_path ? join_path(data_dir, name) : name);
            }
        }
    } while (FindNextFileA(handle, &find_data));

    FindClose(handle);
    return SUCCESS;
#else
    (void)with_path;
    return ERR_FILE_OPEN_FAIL;
#endif
}

/**
 * @brief 设置默认的数据文件路径，自动截断超长路径。
 * @param path 新路径字符串，为 nullptr 或空串时使用 "data/road_network.json"
 */
void FileManager::set_default_path(const char *path)
{
    const char *source = (path != nullptr && path[0] != '\0') ? path : "data/road_network.json";
    std::size_t index = 0;
    for (; index + 1 < sizeof(default_path) && source[index] != '\0'; ++index)
    {
        default_path[index] = source[index];
    }
    default_path[index] = '\0';
}
