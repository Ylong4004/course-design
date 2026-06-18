/*
 * 模块名称  : Qt——城市管理面板实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : 城市增删查表单与列表。直接调用 RoadNetwork API。
 */

#include "city_panel.h"
#include "../common/types.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QGroupBox>

CityPanel::CityPanel(RoadNetwork *network, QWidget *parent)
    : QWidget(parent),
      m_network(network),
      m_edit_id(nullptr),
      m_edit_name(nullptr),
      m_btn_add(nullptr),
      m_btn_delete(nullptr),
      m_list(nullptr),
      m_label_count(nullptr)
{
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    /* ===== 新增城市表单 ===== */
    QGroupBox *form_group = new QGroupBox("新增城市");
    QFormLayout *form = new QFormLayout(form_group);

    m_edit_id   = new QLineEdit();
    m_edit_name = new QLineEdit();
    m_edit_id->setPlaceholderText("正整数，如 1");
    m_edit_name->setPlaceholderText("如 北京");

    form->addRow("城市编号:", m_edit_id);
    form->addRow("城市名称:", m_edit_name);

    m_btn_add = new QPushButton("添加城市");
    form->addRow(m_btn_add);

    main_layout->addWidget(form_group);

    /* ===== 城市列表 ===== */
    QGroupBox *list_group = new QGroupBox("已有城市");
    QVBoxLayout *list_layout = new QVBoxLayout(list_group);

    m_list = new QListWidget();
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    list_layout->addWidget(m_list);

    m_label_count = new QLabel("共 0 个城市");
    list_layout->addWidget(m_label_count);

    m_btn_delete = new QPushButton("删除选中城市");
    m_btn_delete->setEnabled(false);
    m_btn_delete->setStyleSheet("color: red;");
    list_layout->addWidget(m_btn_delete);

    main_layout->addWidget(list_group);

    /* ===== 信号连接 ===== */
    connect(m_btn_add,    &QPushButton::clicked,
            this,         &CityPanel::on_add_clicked);
    connect(m_btn_delete, &QPushButton::clicked,
            this,         &CityPanel::on_delete_clicked);
    connect(m_list,       &QListWidget::itemSelectionChanged,
            this,         &CityPanel::on_selection_changed);

    refresh_list();
}

/* ============================================================ */
/*  公有方法                                                    */
/* ============================================================ */

void CityPanel::focus_add_city()
{
    m_edit_id->setFocus();
    m_edit_id->selectAll();
}

/* ============================================================ */
/*  刷新列表                                                    */
/* ============================================================ */

void CityPanel::refresh_list()
{
    m_list->clear();
    if (!m_network) return;

    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    if (!g) return;

    int *ids = nullptr;
    int count = 0;
    g->get_all_vertex_ids(&ids, &count);

    for (int i = 0; i < count; ++i) {
        City_t city;
        if (g->get_vertex(ids[i], &city) == SUCCESS) {
            QString item_text = QString("[%1] %2")
                .arg(city.id).arg(city.name);
            QListWidgetItem *item = new QListWidgetItem(item_text);
            item->setData(Qt::UserRole, city.id);
            m_list->addItem(item);
        }
    }

    delete[] ids;

    m_label_count->setText(QString("共 %1 个城市").arg(count));
}

/* ============================================================ */
/*  信号槽                                                      */
/* ============================================================ */

void CityPanel::on_add_clicked()
{
    if (!m_network) return;

    bool ok = false;
    int id = m_edit_id->text().toInt(&ok);
    if (!ok || id <= 0) {
        QMessageBox::warning(this, "输入错误",
            "城市编号必须为正整数。");
        return;
    }

    QString name = m_edit_name->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误",
            "城市名称不能为空。");
        return;
    }
    if (name.length() >= MAX_CITY_NAME) {
        QMessageBox::warning(this, "输入错误",
            QString("城市名称最长 %1 字符。")
                .arg(MAX_CITY_NAME - 1));
        return;
    }

    int rc = m_network->add_city(id,
        name.toLocal8Bit().constData());

    if (rc == SUCCESS) {
        m_edit_id->clear();
        m_edit_name->clear();
        refresh_list();
        emit city_changed();
    } else if (rc == ERR_CITY_DUPLICATE) {
        QMessageBox::warning(this, "添加失败",
            "城市编号已存在，请更换编号。");
    } else if (rc == ERR_GRAPH_FULL) {
        QMessageBox::warning(this, "添加失败",
            "城市数量已达上限，无法添加。");
    } else {
        QMessageBox::warning(this, "添加失败",
            QString("错误码: %1").arg(rc));
    }
}

void CityPanel::on_delete_clicked()
{
    QListWidgetItem *item = m_list->currentItem();
    if (!item || !m_network) return;

    int id = item->data(Qt::UserRole).toInt();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除",
        QString("确定要删除城市 [%1] 吗？\n将同时删除关联的所有道路。")
            .arg(id),
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    int rc = m_network->remove_city(id);
    if (rc == SUCCESS) {
        refresh_list();
        emit city_changed();
    } else {
        QMessageBox::warning(this, "删除失败", "城市不存在。");
    }
}

void CityPanel::on_selection_changed()
{
    m_btn_delete->setEnabled(
        m_list->currentItem() != nullptr);
}
