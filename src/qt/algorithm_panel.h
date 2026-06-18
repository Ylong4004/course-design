/*
 * 模块名称  : Qt——算法执行面板
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 图算法选择与执行面板——DFS/BFS/Dijkstra/Floyd/Prim/Kruskal/拓扑排序。
 *             直接调用 algorithms/ 层函数，结果输出到文本区域。
 */

#ifndef ALGORITHM_PANEL_H
#define ALGORITHM_PANEL_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>

#include "../services/road_network.h"

/**
 * @class AlgorithmPanel
 * @brief 算法执行面板
 *
 * 支持算法：
 *   - DFS 深度优先遍历
 *   - BFS 广度优先遍历
 *   - Dijkstra 单源最短路径
 *   - Floyd 多源最短路径
 *   - Prim 最小生成树
 *   - Kruskal 最小生成树
 *   - 拓扑排序
 */
class AlgorithmPanel : public QWidget
{
    Q_OBJECT

public:
    explicit AlgorithmPanel(RoadNetwork *network,
                            QWidget *parent = nullptr);

private slots:
    void on_algorithm_changed(int index);
    void on_execute_clicked();

private:
    void execute_dfs(int start);
    void execute_bfs(int start);
    void execute_dijkstra(int start, int end);
    void execute_floyd();
    void execute_prim();
    void execute_kruskal();
    void execute_topo();

    /** @brief 向输出区追加文本 */
    void append_output(const QString &text);

    RoadNetwork *m_network;

    /* 控件 */
    QComboBox   *m_combo_algo;
    QLineEdit   *m_edit_start;
    QLineEdit   *m_edit_end;
    QLabel      *m_label_start;
    QLabel      *m_label_end;
    QPushButton *m_btn_execute;
    QTextEdit   *m_text_output;
};

#endif /* ALGORITHM_PANEL_H */
