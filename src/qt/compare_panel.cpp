/*
 * 模块名称  : Qt——性能对比面板实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 调用 StructureComparator 四个 measure_* 方法，
 *             以表格形式展示邻接矩阵 vs 邻接表的性能数据。
 */

#include "compare_panel.h"
#include "../common/types.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

ComparePanel::ComparePanel(StructureComparator *comparator,
                           QWidget *parent)
    : QWidget(parent),
      m_comparator(comparator),
      m_btn_run(nullptr),
      m_text_output(nullptr)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    /* ===== 执行按钮 ===== */
    m_btn_run = new QPushButton("运行性能对比");
    m_btn_run->setStyleSheet(
        "QPushButton { background: #FF9800; color: white; "
        "font-weight: bold; padding: 8px; }");
    main_layout->addWidget(m_btn_run);

    /* ===== 结果输出 ===== */
    QGroupBox *out_group = new QGroupBox("对比结果");
    QVBoxLayout *out_layout = new QVBoxLayout(out_group);

    m_text_output = new QTextEdit();
    m_text_output->setReadOnly(true);
    m_text_output->setFont(QFont("Consolas", 10));
    out_layout->addWidget(m_text_output);

    main_layout->addWidget(out_group);

    /* ===== 信号 ===== */
    connect(m_btn_run, &QPushButton::clicked,
            this,      &ComparePanel::on_run_compare);
}

void ComparePanel::on_run_compare()
{
    if (!m_comparator) return;

    m_text_output->clear();
    m_text_output->append("=== 邻接矩阵 vs 邻接表 性能对比 ===\n");

    /* 内存占用 */
    size_t mem_matrix = 0, mem_list = 0;
    m_comparator->measure_memory(&mem_matrix, &mem_list);
    m_text_output->append("【内存占用】");
    m_text_output->append(
        QString("  邻接矩阵: %1 bytes (%2 KB)")
            .arg(mem_matrix)
            .arg(mem_matrix / 1024.0, 0, 'f', 2));
    m_text_output->append(
        QString("  邻接表:   %1 bytes (%2 KB)")
            .arg(mem_list)
            .arg(mem_list / 1024.0, 0, 'f', 2));
    m_text_output->append(
        QString("  差异:     %1 bytes\n")
            .arg((long long)mem_matrix - (long long)mem_list));

    /* 遍历耗时 */
    double t_matrix = 0.0, t_list = 0.0;
    m_comparator->measure_traverse_time(&t_matrix, &t_list);
    m_text_output->append("【遍历耗时（DFS+BFS 全顶点）】");
    m_text_output->append(
        QString("  邻接矩阵: %1 ms").arg(t_matrix, 0, 'f', 4));
    m_text_output->append(
        QString("  邻接表:   %1 ms").arg(t_list, 0, 'f', 4));
    m_text_output->append(
        QString("  比值:     %1x\n")
            .arg(t_list > 0 ? t_matrix / t_list : 0, 0, 'f', 2));

    /* 查找效率 */
    int cmps_matrix = 0, cmps_list = 0;
    m_comparator->measure_find_efficiency(&cmps_matrix, &cmps_list);
    m_text_output->append("【顶点查找比较次数】");
    m_text_output->append(
        QString("  邻接矩阵: %1 次").arg(cmps_matrix));
    m_text_output->append(
        QString("  邻接表:   %1 次\n").arg(cmps_list));

    /* 边查询效率 */
    int eq_matrix = 0, eq_list = 0;
    m_comparator->measure_edge_query(&eq_matrix, &eq_list);
    m_text_output->append("【边查询次数】");
    m_text_output->append(
        QString("  邻接矩阵: %1 次").arg(eq_matrix));
    m_text_output->append(
        QString("  邻接表:   %1 次").arg(eq_list));
}
