cmake_minimum_required(VERSION 3.21)
project(RA2NodeStudio VERSION 0.2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()
qt_add_executable(RA2NodeStudio
    modern/main.cpp
    modern/IniDocument.h modern/IniDocument.cpp
    modern/GraphView.h modern/GraphView.cpp
    modern/GraphNeighborhood.h modern/GraphNeighborhood.cpp
    modern/MainWindow.h modern/MainWindow.cpp
    modern/ThemeStore.h modern/ThemeStore.cpp
)
target_link_libraries(RA2NodeStudio PRIVATE Qt6::Widgets)
set_target_properties(RA2NodeStudio PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
include(GNUInstallDirs)
install(TARGETS RA2NodeStudio BUNDLE DESTINATION . RUNTIME DESTINATION bin)
include(CTest)
if(BUILD_TESTING)
    add_executable(ini_document_tests tests/ini_document_tests.cpp modern/IniDocument.cpp modern/IniDocument.h)
    target_link_libraries(ini_document_tests PRIVATE Qt6::Core)
    add_test(NAME ini_document_tests COMMAND ini_document_tests)
endif()

if(BUILD_TESTING)
    add_executable(theme_tests tests/theme_tests.cpp modern/ThemeStore.cpp modern/ThemeStore.h)
    target_link_libraries(theme_tests PRIVATE Qt6::Widgets)
    add_test(NAME theme_tests COMMAND theme_tests)
endif()

if(BUILD_TESTING)
    add_test(NAME ui_startup_smoke COMMAND RA2NodeStudio --smoke-test)
    set_tests_properties(ui_startup_smoke PROPERTIES ENVIRONMENT
        "QT_QPA_PLATFORM=offscreen;RA2_UI_PREVIEW_PATH=${CMAKE_BINARY_DIR}/ui-preview.png;RA2_THEME_PREVIEW_PATH=${CMAKE_BINARY_DIR}/theme-preview.png")
endif()

if(BUILD_TESTING)
    add_executable(graph_neighborhood_tests
        tests/graph_neighborhood_tests.cpp
        modern/GraphNeighborhood.cpp modern/GraphNeighborhood.h
        modern/IniDocument.cpp modern/IniDocument.h)
    target_link_libraries(graph_neighborhood_tests PRIVATE Qt6::Core)
    add_test(NAME graph_neighborhood_tests COMMAND graph_neighborhood_tests)
endif()
