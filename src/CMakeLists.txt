cmake_minimum_required(VERSION 3.16)
project(NeonCycleExplorer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE APPLICATION_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Logic/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/gui_qt/*.cpp"
)

file(GLOB_RECURSE APPLICATION_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Logic/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/gui_qt/*.h"
)

# Avoid pulling generated files from any CMake build directory that may live inside the source tree.
list(FILTER APPLICATION_SOURCES EXCLUDE REGEX ".*/CMakeFiles/.*")
list(FILTER APPLICATION_HEADERS EXCLUDE REGEX ".*/CMakeFiles/.*")

qt_add_executable(NeonCycleExplorer
    ${APPLICATION_SOURCES}
    ${APPLICATION_HEADERS}
)

qt_add_resources(NeonCycleExplorer resources.qrc)

target_include_directories(NeonCycleExplorer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Logic
    ${CMAKE_CURRENT_SOURCE_DIR}/gui_qt
)

target_link_libraries(NeonCycleExplorer PRIVATE Qt6::Widgets)
