/*
 * 模块名称  : Qt——应用入口实现
 * 编写人    : 组员C（交互&测试负责人）
 * 功能描述  : Qt GUI 主窗口实现——服务初始化、布局组装、信号连接。
 */

#include "qt_app.h"
#include "network_view.h"
#include "city_panel.h"
#include "road_panel.h"
#include "algorithm_panel.h"
#include "congestion_panel.h"
#include "compare_panel.h"

#include "../services/file_io.h"
#include "../common/defines.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

/* ============================================================ */
/*  Qt 模式总入口                                               */
/* ============================================================ */

int qt_run(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("城市交通路网分析系统");

    MainWindow window;
    window.show();

    return app.exec();
}

/* ============================================================ */
/*  主窗口                                                      */
/* ============================================================ */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_network(nullptr),
      m_simulator(nullptr),
      m_comparator(nullptr),
      m_network_view(nullptr),
      m_city_panel(nullptr),
      m_road_panel(nullptr),
      m_algo_panel(nullptr),
      m_congest_panel(nullptr),
      m_compare_panel(nullptr),
      m_act_add_city(nullptr),
      m_act_add_road(nullptr),
      m_act_save(nullptr),
      m_act_load(nullptr),
      m_act_refresh(nullptr),
      m_status_info(nullptr)
{
    setWindowTitle("城市交通路网分析系统");
    resize(1200, 750);

    init_services();
    load_default_data();

    /* 中央：路网可视化画布 */
    m_network_view = new NetworkView(m_network, this);
    setCentralWidget(m_network_view);

    create_actions();
    create_toolbar();
    create_statusbar();
    create_dock_panels();

    /* 初始刷新 */
    on_network_changed();
}

MainWindow::~MainWindow()
{
    safe_delete(m_comparator);
    safe_delete(m_simulator);
    safe_delete(m_network);
}

/* ============================================================ */
/*  服务初始化                                                  */
/* ============================================================ */

void MainWindow::init_services()
{
    safe_new(m_network, RoadNetwork, MAX_CITY_COUNT, GRAPH_UNDIRECTED);

    GraphBase *list_graph   = m_network->get_graph(STORAGE_LIST);
    GraphBase *matrix_graph = m_network->get_graph(STORAGE_MATRIX);

    safe_new(m_simulator, CongestionSimulator, list_graph, 100);
    safe_new(m_comparator, StructureComparator, matrix_graph, list_graph);
}

void MainWindow::load_default_data()
{
    if (FileManager::data_file_exists()) {
        GraphBase *lg = m_network->get_graph(STORAGE_LIST);
        GraphBase *mg = m_network->get_graph(STORAGE_MATRIX);
        FileManager::auto_load(lg);
        FileManager::auto_load(mg);
    }
}

/* ============================================================ */
/*  工具栏 + 菜单栏                                             */
/* ============================================================ */

void MainWindow::create_actions()
{
    m_act_add_city  = new QAction("新增城市", this);
    m_act_add_road  = new QAction("新增道路", this);
    m_act_save      = new QAction("保存路网", this);
    m_act_load      = new QAction("加载路网", this);
    m_act_refresh   = new QAction("刷新视图", this);

    connect(m_act_add_city,  &QAction::triggered,
            this,            &MainWindow::on_add_city);
    connect(m_act_add_road,  &QAction::triggered,
            this,            &MainWindow::on_add_road);
    connect(m_act_save,      &QAction::triggered,
            this,            &MainWindow::on_save_file);
    connect(m_act_load,      &QAction::triggered,
            this,            &MainWindow::on_load_file);
    connect(m_act_refresh,   &QAction::triggered,
            this,            &MainWindow::on_refresh_view);
}

void MainWindow::create_toolbar()
{
    QToolBar *tb = addToolBar("主工具栏");
    tb->setMovable(false);

    tb->addAction(m_act_add_city);
    tb->addAction(m_act_add_road);
    tb->addSeparator();
    tb->addAction(m_act_save);
    tb->addAction(m_act_load);
    tb->addSeparator();
    tb->addAction(m_act_refresh);
}

void MainWindow::create_statusbar()
{
    m_status_info = new QLabel("就绪");
    statusBar()->addWidget(m_status_info);
}

/* ============================================================ */
/*  可停靠面板                                                  */
/* ============================================================ */

void MainWindow::create_dock_panels()
{
    /* 城市管理面板 */
    m_city_panel = new CityPanel(m_network, this);
    QDockWidget *city_dock = new QDockWidget("城市管理", this);
    city_dock->setWidget(m_city_panel);
    city_dock->setAllowedAreas(Qt::LeftDockWidgetArea
                               | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, city_dock);

    connect(m_city_panel, &CityPanel::city_changed,
            this,          &MainWindow::on_network_changed);

    /* 道路管理面板 */
    m_road_panel = new RoadPanel(m_network, this);
    QDockWidget *road_dock = new QDockWidget("道路管理", this);
    road_dock->setWidget(m_road_panel);
    road_dock->setAllowedAreas(Qt::LeftDockWidgetArea
                               | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, road_dock);

    connect(m_road_panel, &RoadPanel::road_changed,
            this,          &MainWindow::on_network_changed);

    /* 算法面板 */
    m_algo_panel = new AlgorithmPanel(m_network, this);
    QDockWidget *algo_dock = new QDockWidget("算法执行", this);
    algo_dock->setWidget(m_algo_panel);
    algo_dock->setAllowedAreas(Qt::LeftDockWidgetArea
                               | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, algo_dock);

    /* 拥堵模拟面板 */
    m_congest_panel = new CongestionPanel(m_network, m_simulator, this);
    QDockWidget *congest_dock = new QDockWidget("拥堵模拟", this);
    congest_dock->setWidget(m_congest_panel);
    congest_dock->setAllowedAreas(Qt::LeftDockWidgetArea
                                  | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, congest_dock);

    /* 性能对比面板 */
    m_compare_panel = new ComparePanel(m_comparator, this);
    QDockWidget *compare_dock = new QDockWidget("性能对比", this);
    compare_dock->setWidget(m_compare_panel);
    compare_dock->setAllowedAreas(Qt::LeftDockWidgetArea
                                  | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, compare_dock);
}

/* ============================================================ */
/*  工具栏动作实现                                              */
/* ============================================================ */

void MainWindow::on_add_city()
{
    if (m_city_panel) {
        m_city_panel->focus_add_city();
    }
}

void MainWindow::on_add_road()
{
    if (m_road_panel) {
        m_road_panel->focus_add_road();
    }
}

void MainWindow::on_save_file()
{
    QString path = QFileDialog::getSaveFileName(
        this, "保存路网", "../data/", "路网文件 (*.txt)");
    if (path.isEmpty()) return;

    GraphBase *g = m_network->get_graph(STORAGE_LIST);
    int rc = FileManager::save_to_file(g,
        path.toLocal8Bit().constData());
    if (rc == SUCCESS) {
        statusBar()->showMessage("已保存: " + path, 3000);
    } else {
        QMessageBox::warning(this, "保存失败",
            "文件保存失败，请检查路径权限。");
    }
}

void MainWindow::on_load_file()
{
    QString path = QFileDialog::getOpenFileName(
        this, "加载路网", "../data/", "路网文件 (*.txt)");
    if (path.isEmpty()) return;

    GraphBase *lg = m_network->get_graph(STORAGE_LIST);
    GraphBase *mg = m_network->get_graph(STORAGE_MATRIX);

    int rc = FileManager::load_from_file(lg,
        path.toLocal8Bit().constData());
    if (rc == SUCCESS) {
        FileManager::load_from_file(mg,
            path.toLocal8Bit().constData());
    }

    if (rc == SUCCESS) {
        on_network_changed();
        statusBar()->showMessage("已加载: " + path, 3000);
    } else {
        QMessageBox::warning(this, "加载失败",
            "文件格式错误或文件不存在。");
    }
}

void MainWindow::on_refresh_view()
{
    on_network_changed();
}

/* ============================================================ */
/*  数据变更 → 视图刷新                                         */
/* ============================================================ */

void MainWindow::on_network_changed()
{
    /* 刷新路网画布 */
    if (m_network_view) {
        m_network_view->refresh();
    }

    /* 刷新状态栏 */
    int city_count = 0;
    int road_count = 0;
    if (m_network) {
        city_count = m_network->get_city_count();
        road_count = m_network->get_road_count();
    }

    if (m_status_info) {
        m_status_info->setText(
            QString("城市: %1  |  道路: %2  |  类型: 无向图")
                .arg(city_count).arg(road_count));
    }
}
