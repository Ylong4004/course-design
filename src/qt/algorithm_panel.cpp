/*
 * 模块名称  : Qt——算法执行面板实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 7 种图算法的选择、参数输入与结果展示。
 *             直接调用 algorithms/ 层，结果以文本形式输出。
 */

#include "algorithm_panel.h"

#include "../algorithms/traversal.h"
#include "../algorithms/shortest_path.h"
#include "../algorithms/spanning_tree.h"
#include "../algorithms/topological.h"
#include "../common/types.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollBar>

AlgorithmPanel::AlgorithmPanel(RoadNetwork *network, QWidget *parent)
    : QWidget(parent),
      m_network(network),
      m_combo_algo(nullptr),
      m_edit_start(nullptr),
      m_edit_end(nullptr),
      m_label_start(nullptr),
      m_label_end(nullptr),
      m_btn_execute(nullptr),
      m_text_output(nullptr)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    /* ===== 算法选择 ===== */
    QGroupBox *select_group = new QGroupBox("选择算法");
    QFormLayout *select_form = new QFormLayout(select_group);

    m_combo_algo = new QComboBox();
    m_combo_algo->addItem("DFS 深度优先遍历",         0);
    m_combo_algo->addItem("BFS 广度优先遍历",         1);
    m_combo_algo->addItem("Dijkstra 单源最短路径",    2);
    m_combo_algo->addItem("Floyd 多源最短路径",       3);
    m_combo_algo->addItem("Prim 最小生成树",          4);
    m_combo_algo->addItem("Kruskal 最小生成树",       5);
    m_combo_algo->addItem("拓扑排序",                 6);
    select_form->addRow("算法:", m_combo_algo);

    main_layout->addWidget(select_group);

    /* ===== 参数输入 ===== */
    QGroupBox *param_group = new QGroupBox("参数");
    QFormLayout *param_form = new QFormLayout(param_group);

    m_label_start = new QLabel("起点城市编号:");
    m_label_end   = new QLabel("终点城市编号（选填）:");
    m_edit_start  = new QLineEdit();
    m_edit_end    = new QLineEdit();
    m_edit_start->setPlaceholderText("如 1");
    m_edit_end->setPlaceholderText("Dijkstra 可选终点");

    param_form->addRow(m_label_start, m_edit_start);
    param_form->addRow(m_label_end,   m_edit_end);

    main_layout->addWidget(param_group);

    /* ===== 执行按钮 ===== */
    m_btn_execute = new QPushButton("执行算法");
    m_btn_execute->setStyleSheet(
        "QPushButton { background: #4CAF50; color: white; "
        "font-weight: bold; padding: 6px; }");
    main_layout->addWidget(m_btn_execute);

    /* ===== 结果输出 ===== */
    QGroupBox *output_group = new QGroupBox("结果");
    QVBoxLayout *output_layout = new QVBoxLayout(output_group);

    m_text_output = new QTextEdit();
    m_text_output->setReadOnly(true);
    m_text_output->setFont(QFont("Consolas", 10));
    output_layout->addWidget(m_text_output);

    main_layout->addWidget(output_group);

    /* ===== 信号连接 ===== */
    connect(m_btn_execute, &QPushButton::clicked,
            this,          &AlgorithmPanel::on_execute_clicked);
    connect(m_combo_algo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AlgorithmPanel::on_algorithm_changed);

    /* 初始状态 */
    on_algorithm_changed(m_combo_algo->currentIndex());
}

/* ============================================================ */
/*  算法切换 → 参数可见性                                       */
/* ============================================================ */

void AlgorithmPanel::on_algorithm_changed(int index)
{
    (void)index;
    int algo = m_combo_algo->currentData().toInt();

    /* Floyd / Prim / Kruskal / Topo 不需要起点终点 */
    bool need_start = (algo != 3 && algo != 4
                       && algo != 5 && algo != 6);
    bool need_end   = (algo == 2); /* 仅 Dijkstra 可选终点 */

    m_label_start->setVisible(need_start);
    m_edit_start->setVisible(need_start);
    m_label_end->setVisible(need_end);
    m_edit_end->setVisible(need_end);
}

/* ============================================================ */
/*  执行算法                                                    */
/* ============================================================ */

void AlgorithmPanel::on_execute_clicked()
{
    m_text_output->clear();

    if (!m_network) return;

    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    if (!g || g->get_vertex_count() == 0) {
        append_output("路网为空，请先添加城市和道路。");
        return;
    }

    int algo = m_combo_algo->currentData().toInt();
    bool ok  = false;
    int start = m_edit_start->text().toInt(&ok);

    /* 需要起点的算法：校验输入 */
    if (algo != 3 && algo != 4 && algo != 5 && algo != 6) {
        if (!ok || start <= 0) {
            QMessageBox::warning(this, "输入错误",
                "请输入有效的起点城市编号。");
            return;
        }
    }

    switch (algo) {
    case 0: execute_dfs(start);                          break;
    case 1: execute_bfs(start);                          break;
    case 2: {
        int end = m_edit_end->text().toInt(&ok);
        execute_dijkstra(start, ok ? end : -1);
        break;
    }
    case 3: execute_floyd();                             break;
    case 4: execute_prim();                              break;
    case 5: execute_kruskal();                           break;
    case 6: execute_topo();                              break;
    default: break;
    }
}

/* ============================================================ */
/*  各算法实现                                                  */
/* ============================================================ */

void AlgorithmPanel::execute_dfs(int start)
{
    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    int *seq = nullptr;
    int len  = 0;

    int rc = traverse_dfs(g, start, &seq, &len);
    if (rc == SUCCESS && len > 0) {
        append_output("=== DFS 深度优先遍历 ===\n");
        for (int i = 0; i < len; ++i) {
            City_t city;
            g->get_vertex(seq[i], &city);
            append_output(QString("  %1. [%2] %3")
                .arg(i + 1).arg(city.id).arg(city.name));
        }
    } else {
        append_output("DFS 遍历失败（起始城市不存在或图为空）。");
    }
    delete[] seq;
}

void AlgorithmPanel::execute_bfs(int start)
{
    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    int *seq = nullptr;
    int len  = 0;

    int rc = traverse_bfs(g, start, &seq, &len);
    if (rc == SUCCESS && len > 0) {
        append_output("=== BFS 广度优先遍历 ===\n");
        for (int i = 0; i < len; ++i) {
            City_t city;
            g->get_vertex(seq[i], &city);
            append_output(QString("  %1. [%2] %3")
                .arg(i + 1).arg(city.id).arg(city.name));
        }
    } else {
        append_output("BFS 遍历失败（起始城市不存在或图为空）。");
    }
    delete[] seq;
}

void AlgorithmPanel::execute_dijkstra(int start, int end)
{
    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    int vcount = g->get_vertex_count();
    int *dist  = new int[vcount];
    int *prev  = new int[vcount];

    int rc = run_dijkstra(g, start, dist, prev);
    if (rc != SUCCESS) {
        append_output("Dijkstra 算法执行失败。");
        delete[] dist; delete[] prev; return;
    }

    append_output(QString("=== Dijkstra 单源最短路径 (起点=%1) ===\n")
        .arg(start));

    if (end > 0) {
        /* 单条路径 */
        int *path = nullptr;
        int plen  = 0;
        if (dijkstra_get_path(prev, vcount, start, end,
                              &path, &plen) == SUCCESS && plen > 0) {
            City_t c1, c2;
            g->get_vertex(start, &c1);
            g->get_vertex(end, &c2);
            append_output(QString("  %1 → %2  距离: %3")
                .arg(c1.name).arg(c2.name).arg(dist[end]));
            QString route = "  路径: ";
            for (int i = 0; i < plen; ++i) {
                City_t cn;
                g->get_vertex(path[i], &cn);
                route += cn.name;
                if (i + 1 < plen) route += " → ";
            }
            append_output(route);
        } else {
            append_output(QString("  城市 %1 不可达。").arg(end));
        }
        delete[] path;
    } else {
        /* 全量输出 */
        for (int i = 0; i < vcount; ++i) {
            City_t c;
            if (g->get_vertex(i, &c) == SUCCESS) {
                append_output(QString("  → %1 [%2]: 距离 %3")
                    .arg(c.name).arg(c.id)
                    .arg(dist[i] == INF_WEIGHT ? -1 : dist[i]));
            }
        }
    }

    delete[] dist;
    delete[] prev;
}

void AlgorithmPanel::execute_floyd()
{
    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    int vcount = g->get_vertex_count();
    int **dist = nullptr;
    int **next = nullptr;

    int rc = run_floyd(g, &dist, &next, &vcount);
    if (rc != SUCCESS) {
        append_output("Floyd 算法执行失败。");
        return;
    }

    append_output("=== Floyd 多源最短路径 ===\n");
    append_output(QString("顶点数: %1\n").arg(vcount));

    /* 表头 */
    QString header = "     ";
    for (int j = 0; j < vcount; ++j) {
        City_t c;
        g->get_vertex(j, &c);
        header += QString("%1 ").arg(c.name, 6);
    }
    append_output(header);

    for (int i = 0; i < vcount; ++i) {
        City_t ci;
        g->get_vertex(i, &ci);
        QString row = QString("%1 ").arg(ci.name, -4);
        for (int j = 0; j < vcount; ++j) {
            if (dist[i][j] == INF_WEIGHT) {
                row += "   ∞  ";
            } else {
                row += QString("%1 ").arg(dist[i][j], 5);
            }
        }
        append_output(row);
    }

    for (int i = 0; i < vcount; ++i) { delete[] dist[i]; delete[] next[i]; }
    delete[] dist; delete[] next;
}

void AlgorithmPanel::execute_prim()
{
    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    MSTResult_t mst;
    std::memset(&mst, 0, sizeof(mst));

    int rc = build_mst_prim(g, &mst);
    if (rc == SUCCESS) {
        append_output("=== Prim 最小生成树 ===\n");
        append_output(QString("总造价: %1\n").arg(mst.total_cost));
        for (int i = 0; i < mst.edge_count; ++i) {
            append_output(QString("  %1 → %2  权值: %3")
                .arg(mst.edges[i].from)
                .arg(mst.edges[i].to)
                .arg(mst.edges[i].weight));
        }
    } else if (rc == ERR_DISCONNECTED) {
        append_output("图不连通，无法生成最小生成树。");
    } else {
        append_output("Prim 算法执行失败。");
    }
    free_mst_result(&mst);
}

void AlgorithmPanel::execute_kruskal()
{
    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    MSTResult_t mst;
    std::memset(&mst, 0, sizeof(mst));

    int rc = build_mst_kruskal(g, &mst);
    if (rc == SUCCESS) {
        append_output("=== Kruskal 最小生成树 ===\n");
        append_output(QString("总造价: %1\n").arg(mst.total_cost));
        for (int i = 0; i < mst.edge_count; ++i) {
            append_output(QString("  %1 → %2  权值: %3")
                .arg(mst.edges[i].from)
                .arg(mst.edges[i].to)
                .arg(mst.edges[i].weight));
        }
    } else if (rc == ERR_DISCONNECTED) {
        append_output("图不连通，无法生成最小生成树。");
    } else {
        append_output("Kruskal 算法执行失败。");
    }
    free_mst_result(&mst);
}

void AlgorithmPanel::execute_topo()
{
    GraphBase *g = m_network->get_graph(STORAGE_LIST);

    if (g->get_graph_type() != GRAPH_DIRECTED) {
        append_output("拓扑排序仅适用于有向图，当前路网为无向图。");
        return;
    }

    int *seq  = nullptr;
    int len   = 0;
    bool cycle = false;

    int rc = run_topological_sort(g, &seq, &len, &cycle);
    if (rc != SUCCESS) {
        append_output("拓扑排序执行失败。");
        delete[] seq; return;
    }

    append_output("=== 拓扑排序 ===\n");
    if (cycle) {
        append_output("图中存在环路，无法完全拓扑排序。\n");
    }
    for (int i = 0; i < len; ++i) {
        City_t city;
        g->get_vertex(seq[i], &city);
        append_output(QString("  %1. [%2] %3")
            .arg(i + 1).arg(city.id).arg(city.name));
    }
    delete[] seq;
}

/* ============================================================ */
/*  输出辅助                                                    */
/* ============================================================ */

void AlgorithmPanel::append_output(const QString &text)
{
    m_text_output->append(text);
}
