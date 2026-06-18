/*
 * 模块名称  : Qt——性能对比面板
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 邻接矩阵 vs 邻接表性能对比——调用 StructureComparator 四个 measure_* 方法。
 */

#ifndef COMPARE_PANEL_H
#define COMPARE_PANEL_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>

#include "../services/comparator.h"

/**
 * @class ComparePanel
 * @brief 性能对比面板
 */
class ComparePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ComparePanel(StructureComparator *comparator,
                          QWidget *parent = nullptr);

private slots:
    void on_run_compare();

private:
    StructureComparator *m_comparator;

    QPushButton *m_btn_run;
    QTextEdit   *m_text_output;
};

#endif /* COMPARE_PANEL_H */
