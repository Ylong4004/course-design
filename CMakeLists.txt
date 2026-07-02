# ============================================================
#  城市交通路网分析系统 — CMake 构建文件
#  支持两种构建目标：
#    1. 默认目标 (traffic_network)   — 菜单/CLI/Qt 全包含
#    2. 无 GUI 目标 (traffic_console) — 仅菜单/CLI，不依赖 Qt
# ============================================================

cmake_minimum_required(VERSION 3.16)
project(TrafficNetwork VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_QT_GUI "Build Qt GUI target" OFF)

# ----------------------------------------------------------
#  查找 Qt（优先 Qt6，回退 Qt5；未安装 Qt 时仍可编译控制台版本）
# ----------------------------------------------------------
if(BUILD_QT_GUI)
    find_package(Qt6 COMPONENTS Core Gui Widgets QUIET)
    if(NOT Qt6_FOUND)
        find_package(Qt5 COMPONENTS Core Gui Widgets QUIET)
    endif()
endif()

# ----------------------------------------------------------
#  公共源文件（不含 Qt 模块）
# ----------------------------------------------------------
set(COMMON_SOURCES
    src/main.cpp
    # common
    # （common/ 只有头文件，无需编译）
    # graph
    src/graph/adj_matrix.cpp
    src/graph/adj_list.cpp
    # algorithms
    src/algorithms/queue.cpp
    src/algorithms/stack.cpp
    src/algorithms/union_find.cpp
    src/algorithms/priority_queue.cpp
    src/algorithms/traversal.cpp
    src/algorithms/shortest_path.cpp
    src/algorithms/spanning_tree.cpp
    src/algorithms/topological.cpp
    # services
    src/services/road_network.cpp
    src/services/congestion.cpp
    src/services/comparator.cpp
    src/services/file_io.cpp
    # ui (控制台)
    src/ui/menu.cpp
    src/ui/formatter.cpp
    src/ui/validator.cpp
    # cli
    src/cli/cli_app.cpp
    src/cli/command_parser.cpp
    # test
    src/test/test_cases.cpp
)

# ----------------------------------------------------------
#  Qt 源文件（含 Q_OBJECT，需要 MOC 处理）
# ----------------------------------------------------------
set(QT_SOURCES
    src/qt/qt_app.cpp
    src/qt/network_view.cpp
    src/qt/city_panel.cpp
    src/qt/road_panel.cpp
    src/qt/algorithm_panel.cpp
    src/qt/congestion_panel.cpp
    src/qt/compare_panel.cpp
)

# 需要 MOC 处理的头文件（含 Q_OBJECT 宏）
set(QT_HEADERS
    src/qt/qt_app.h
    src/qt/network_view.h
    src/qt/city_panel.h
    src/qt/road_panel.h
    src/qt/algorithm_panel.h
    src/qt/congestion_panel.h
    src/qt/compare_panel.h
)

# ----------------------------------------------------------
#  统一 Qt 版本变量
# ----------------------------------------------------------
if(BUILD_QT_GUI AND Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
    set(Qt_FOUND TRUE)
elseif(BUILD_QT_GUI AND Qt5_FOUND)
    set(QT_VERSION_MAJOR 5)
    set(Qt_FOUND TRUE)
else()
    set(Qt_FOUND FALSE)
endif()

# ----------------------------------------------------------
#  目标 1：完整版（含 Qt GUI）
# ----------------------------------------------------------
if(Qt_FOUND)
    message(STATUS "Qt${QT_VERSION_MAJOR} found, building WITH Qt GUI support")

    # 启用 Qt 自动处理：MOC（信号/槽），只作用于 GUI 目标
    #set(CMAKE_AUTOUIC ON)   # 如有 .ui 文件再启用
    #set(CMAKE_AUTORCC ON)   # 如有 .qrc 文件再启用

    add_executable(traffic_network
        ${COMMON_SOURCES}
        ${QT_SOURCES}
        ${QT_HEADERS}
    )

    target_include_directories(traffic_network PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )

    target_compile_definitions(traffic_network PRIVATE QT_GUI_ENABLED)

    set_target_properties(traffic_network PROPERTIES
        AUTOMOC ON
    )

    target_link_libraries(traffic_network PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
    )

    # Windows 下隐藏控制台窗口（GUI 程序）
    if(WIN32)
        set_target_properties(traffic_network PROPERTIES
            WIN32_EXECUTABLE FALSE  # 设为 TRUE 可隐藏控制台，FALSE 则保留
        )
    endif()

else()
    message(STATUS "Qt NOT found — skipping GUI target")
endif()

# ----------------------------------------------------------
#  目标 2：纯控制台版（不依赖 Qt）
# ----------------------------------------------------------
add_executable(traffic_console
    ${COMMON_SOURCES}
)

target_include_directories(traffic_console PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

# ----------------------------------------------------------
#  输出目录
# ----------------------------------------------------------
set_target_properties(traffic_console PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin
)
if(Qt_FOUND)
    set_target_properties(traffic_network PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/bin
    )
endif()

# ----------------------------------------------------------
#  构建提示
# ----------------------------------------------------------
message(STATUS "")
message(STATUS "=== 构建说明 ===")
message(STATUS "  mingw32-make traffic_console   — 纯控制台版（菜单/CLI）")
if(Qt_FOUND)
    message(STATUS "  mingw32-make traffic_network   — 完整版（含 Qt GUI）")
    message(STATUS "  运行:  ./traffic_network --gui   启动 GUI 模式")
endif()
message(STATUS "  运行:  ./traffic_console          启动菜单模式")
message(STATUS "  运行:  ./traffic_console --cli    启动 CLI 模式")
message(STATUS "")
