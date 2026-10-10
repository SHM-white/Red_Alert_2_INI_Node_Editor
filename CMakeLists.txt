cmake_minimum_required(VERSION 3.24)
project(RA2IniNodeEditor VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(CTest)

add_library(ra2_ini_core STATIC src/core/IniDocument.cpp src/core/IniDocument.h)
target_include_directories(ra2_ini_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/core")
if(MSVC)
    target_compile_options(ra2_ini_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(ra2_ini_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

option(RA2_BUILD_GUI "Build Qt Widgets GUI" ON)
if(RA2_BUILD_GUI)
    find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
    qt_standard_project_setup()
    qt_add_executable(RA2IniNodeEditor WIN32
        src/main.cpp
        src/ui/EditorWindow.h
        src/ui/EditorWindow.cpp
        src/ui/NodeGraphView.h
        src/ui/NodeGraphView.cpp
    )
    target_link_libraries(RA2IniNodeEditor PRIVATE ra2_ini_core Qt6::Widgets)
    if(MSVC)
        target_compile_options(RA2IniNodeEditor PRIVATE /utf-8 /W4 /permissive-)
    endif()
    set_target_properties(RA2IniNodeEditor PROPERTIES
        OUTPUT_NAME "RA2-INI-Node-Editor"
        MACOSX_BUNDLE TRUE
    )
    install(TARGETS RA2IniNodeEditor BUNDLE DESTINATION . RUNTIME DESTINATION bin)
endif()

if(BUILD_TESTING)
    add_executable(ra2_ini_tests tests/test_ini.cpp)
    target_link_libraries(ra2_ini_tests PRIVATE ra2_ini_core)
    add_test(NAME ini_roundtrip COMMAND ra2_ini_tests)
endif()
