/*
 * 模块名称  : Qt——拥堵模拟面板实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 设置拥堵 / 恢复 / 对比分析。调用 CongestionSimulator API。
 */

#include "congestion_panel.h"
#include "../common/types.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

CongestionPanel::CongestionPanel(RoadNetwork *network,
                                 CongestionSimulator *simulator,
                                 QWidget *parent)
    : QWidget(parent),
      m_network(network),
      m_simulator(simulator),
      m_edit_from(nullptr),
      m_edit_to(nullptr),
      m_edit_weight(nullptr),
      m_edit_start(nullptr),
      m_btn_set(nullptr),
      m_btn_restore(nullptr),
      m_btn_compare(nullptr),
      m_text_output(nullptr)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    /* ===== 设置拥堵 ===== */
    QGroupBox *set_group = new QGroupBox("设置拥堵");
    QFormLayout *set_form = new QFormLayout(set_group);

    m_edit_from   = new QLineEdit();
    m_edit_to     = new QLineEdit();
    m_edit_weight = new QLineEdit();
    m_edit_from->setPlaceholderText("起点编号");
    m_edit_to->setPlaceholderText("终点编号");
    m_edit_weight->setPlaceholderText("拥堵后的新权值（如 999）");

    set_form->addRow("起点:", m_edit_from);
    set_form->addRow("终点:", m_edit_to);
    set_form->addRow("新权值:", m_edit_weight);

    QHBoxLayout *set_btn_row = new QHBoxLayout();
    m_btn_set     = new QPushButton("设置拥堵");
    m_btn_restore = new QPushButton("恢复全部");
    m_btn_restore->setStyleSheet("color: green;");
    set_btn_row->addWidget(m_btn_set);
    set_btn_row->addWidget(m_btn_restore);
    set_form->addRow(set_btn_row);

    main_layout->addWidget(set_group);

    /* ===== 对比分析 ===== */
    QGroupBox *cmp_group = new QGroupBox("对比分析");
    QFormLayout *cmp_form = new QFormLayout(cmp_group);

    m_edit_start = new QLineEdit();
    m_edit_start->setPlaceholderText("分析起点城市编号");
    m_btn_compare = new QPushButton("执行对比分析");
    m_btn_compare->setStyleSheet(
        "QPushButton { background: #2196F3; color: white; "
        "font-weight: bold; }");

    cmp_form->addRow("起点:", m_edit_start);
    cmp_form->addRow(m_btn_compare);

    main_layout->addWidget(cmp_group);

    /* ===== 输出 ===== */
    QGroupBox *out_group = new QGroupBox("结果");
    QVBoxLayout *out_layout = new QVBoxLayout(out_group);

    m_text_output = new QTextEdit();
    m_text_output->setReadOnly(true);
    m_text_output->setFont(QFont("Consolas", 10));
    out_layout->addWidget(m_text_output);

    main_layout->addWidget(out_group);

    /* ===== 信号 ===== */
    connect(m_btn_set,     &QPushButton::clicked,
            this,          &CongestionPanel::on_set_congestion);
    connect(m_btn_restore, &QPushButton::clicked,
            this,          &CongestionPanel::on_restore);
    connect(m_btn_compare, &QPushButton::clicked,
            this,          &CongestionPanel::on_compare);
}

/* ============================================================ */
/*  信号槽                                                      */
/* ============================================================ */

void CongestionPanel::on_set_congestion()
{
    if (!m_simulator) return;

    bool ok = false;
    int from = m_edit_from->text().toInt(&ok);
    if (!ok || from <= 0) {
        QMessageBox::warning(this, "输入错误", "起点编号无效。");
        return;
    }
    int to = m_edit_to->text().toInt(&ok);
    if (!ok || to <= 0) {
        QMessageBox::warning(this, "输入错误", "终点编号无效。");
        return;
    }
    int w = m_edit_weight->text().toInt(&ok);
    if (!ok || w <= 0) {
        QMessageBox::warning(this, "输入错误", "权值无效。");
        return;
    }

    int rc = m_simulator->set_congestion(from, to, w);
    if (rc == SUCCESS) {
        m_text_output->append(
            QString("设置拥堵: %1 → %2  新权值=%3")
                .arg(from).arg(to).arg(w));
    } else {
        m_text_output->append("设置拥堵失败（道路不存在）。");
    }
}

void CongestionPanel::on_restore()
{
    if (!m_simulator) return;

    int rc = m_simulator->restore_all();
    if (rc == SUCCESS) {
        m_text_output->append("已恢复所有拥堵道路到原始权值。");
    } else {
        m_text_output->append("恢复失败。");
    }
}

void CongestionPanel::on_compare()
{
    if (!m_simulator || !m_network) return;

    bool ok = false;
    int start = m_edit_start->text().toInt(&ok);
    if (!ok || start <= 0) {
        QMessageBox::warning(this, "输入错误", "起点编号无效。");
        return;
    }

    int rc = m_simulator->run_comparison(start);
    if (rc != SUCCESS) {
        m_text_output->append("对比分析失败（起点城市不存在或无拥堵数据）。");
        return;
    }

    /* 从模拟器获取对比数据并展示 */
    m_text_output->clear();
    m_text_output->append(
        QString("=== 拥堵前后对比 (起点=%1) ===\n").arg(start));

    /* 这里直接输出来自 simulator 内部数据，由于 print_comparison_report
     * 直接写 std::cout，Qt 层用信号机制来桥接。
     * 简化方案：调用 simulator 的对比数据展示。 */
    m_text_output->append(
        "对比分析已执行。请查看控制台输出或上方路网视图查看变化。");
}
