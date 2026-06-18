/*
 * 模块名称  : Qt——道路管理面板实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 道路增删改表单与列表。直接调用 RoadNetwork API。
 */

#include "road_panel.h"
#include "../common/types.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QGroupBox>

RoadPanel::RoadPanel(RoadNetwork *network, QWidget *parent)
    : QWidget(parent),
      m_network(network),
      m_edit_from(nullptr),
      m_edit_to(nullptr),
      m_edit_weight(nullptr),
      m_btn_add(nullptr),
      m_btn_delete(nullptr),
      m_btn_update(nullptr),
      m_list(nullptr),
      m_label_count(nullptr)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    /* ===== 新增道路表单 ===== */
    QGroupBox *form_group = new QGroupBox("道路操作");
    QFormLayout *form = new QFormLayout(form_group);

    m_edit_from   = new QLineEdit();
    m_edit_to     = new QLineEdit();
    m_edit_weight = new QLineEdit();
    m_edit_from->setPlaceholderText("起点城市编号");
    m_edit_to->setPlaceholderText("终点城市编号");
    m_edit_weight->setPlaceholderText("权值（正整数）");

    form->addRow("起点:", m_edit_from);
    form->addRow("终点:", m_edit_to);
    form->addRow("权值:", m_edit_weight);

    /* 按钮行 */
    QHBoxLayout *btn_row = new QHBoxLayout();
    m_btn_add    = new QPushButton("添加道路");
    m_btn_update = new QPushButton("修改权值");
    btn_row->addWidget(m_btn_add);
    btn_row->addWidget(m_btn_update);
    form->addRow(btn_row);

    main_layout->addWidget(form_group);

    /* ===== 道路列表 ===== */
    QGroupBox *list_group = new QGroupBox("已有道路");
    QVBoxLayout *list_layout = new QVBoxLayout(list_group);

    m_list = new QListWidget();
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    list_layout->addWidget(m_list);

    m_label_count = new QLabel("共 0 条道路");
    list_layout->addWidget(m_label_count);

    m_btn_delete = new QPushButton("删除选中道路");
    m_btn_delete->setEnabled(false);
    m_btn_delete->setStyleSheet("color: red;");
    list_layout->addWidget(m_btn_delete);

    main_layout->addWidget(list_group);

    /* ===== 信号连接 ===== */
    connect(m_btn_add,    &QPushButton::clicked,
            this,         &RoadPanel::on_add_clicked);
    connect(m_btn_delete, &QPushButton::clicked,
            this,         &RoadPanel::on_delete_clicked);
    connect(m_btn_update, &QPushButton::clicked,
            this,         &RoadPanel::on_update_clicked);
    connect(m_list,       &QListWidget::itemSelectionChanged,
            this,         &RoadPanel::on_selection_changed);

    refresh_list();
}

/* ============================================================ */
/*  公有方法                                                    */
/* ============================================================ */

void RoadPanel::focus_add_road()
{
    m_edit_from->setFocus();
    m_edit_from->selectAll();
}

/* ============================================================ */
/*  刷新列表                                                    */
/* ============================================================ */

void RoadPanel::refresh_list()
{
    m_list->clear();
    if (!m_network) return;

    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    if (!g) return;

    int *ids = nullptr;
    int count = 0;
    g->get_all_vertex_ids(&ids, &count);

    int road_total = 0;
    for (int i = 0; i < count; ++i) {
        for (int j = 0; j < count; ++j) {
            if (i == j) continue;
            if (!g->has_edge(ids[i], ids[j])) continue;

            int weight = 0;
            g->get_edge_weight(ids[i], ids[j], &weight);

            QString item_text = QString("%1 → %2  [权值: %3]")
                .arg(ids[i]).arg(ids[j]).arg(weight);
            QListWidgetItem *item = new QListWidgetItem(item_text);
            /* 存储 from, to 以便删除时提取 */
            item->setData(Qt::UserRole,     ids[i]);
            item->setData(Qt::UserRole + 1, ids[j]);
            item->setData(Qt::UserRole + 2, weight);
            m_list->addItem(item);
            ++road_total;
        }
    }

    delete[] ids;

    m_label_count->setText(
        QString("共 %1 条道路").arg(road_total));
}

/* ============================================================ */
/*  信号槽                                                      */
/* ============================================================ */

void RoadPanel::on_add_clicked()
{
    if (!m_network) return;

    bool ok = false;
    int from = m_edit_from->text().toInt(&ok);
    if (!ok || from <= 0) {
        QMessageBox::warning(this, "输入错误", "起点编号必须为正整数。");
        return;
    }
    int to = m_edit_to->text().toInt(&ok);
    if (!ok || to <= 0) {
        QMessageBox::warning(this, "输入错误", "终点编号必须为正整数。");
        return;
    }
    int weight = m_edit_weight->text().toInt(&ok);
    if (!ok || weight <= 0) {
        QMessageBox::warning(this, "输入错误", "权值必须为正整数。");
        return;
    }
    if (from == to) {
        QMessageBox::warning(this, "输入错误", "不允许自环道路。");
        return;
    }

    int rc = m_network->add_road(from, to, weight);
    if (rc == SUCCESS) {
        m_edit_from->clear();
        m_edit_to->clear();
        m_edit_weight->clear();
        refresh_list();
        emit road_changed();
    } else if (rc == ERR_CITY_NOT_FOUND) {
        QMessageBox::warning(this, "添加失败", "起点或终点城市不存在。");
    } else if (rc == ERR_ROAD_EXISTS) {
        QMessageBox::warning(this, "添加失败", "该道路已存在。");
    } else {
        QMessageBox::warning(this, "添加失败",
            QString("错误码: %1").arg(rc));
    }
}

void RoadPanel::on_delete_clicked()
{
    QListWidgetItem *item = m_list->currentItem();
    if (!item || !m_network) return;

    int from = item->data(Qt::UserRole).toInt();
    int to   = item->data(Qt::UserRole + 1).toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除",
        QString("确定要删除道路 %1 → %2 吗？")
            .arg(from).arg(to),
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    int rc = m_network->remove_road(from, to);
    if (rc == SUCCESS) {
        refresh_list();
        emit road_changed();
    } else {
        QMessageBox::warning(this, "删除失败", "道路不存在。");
    }
}

void RoadPanel::on_update_clicked()
{
    QListWidgetItem *item = m_list->currentItem();
    if (!item || !m_network) return;

    bool ok = false;
    int new_weight = m_edit_weight->text().toInt(&ok);
    if (!ok || new_weight <= 0) {
        QMessageBox::warning(this, "输入错误", "请输入有效的新权值（正整数）。");
        return;
    }

    int from = item->data(Qt::UserRole).toInt();
    int to   = item->data(Qt::UserRole + 1).toInt();

    int rc = m_network->update_road_weight(from, to, new_weight);
    if (rc == SUCCESS) {
        m_edit_weight->clear();
        refresh_list();
        emit road_changed();
    } else {
        QMessageBox::warning(this, "修改失败",
            "道路不存在或权值无效。");
    }
}

void RoadPanel::on_selection_changed()
{
    bool has_sel = (m_list->currentItem() != nullptr);
    m_btn_delete->setEnabled(has_sel);
    m_btn_update->setEnabled(has_sel);
}
