cmake_minimum_required(VERSION 3.16)
project(NeonCycleExplorer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Source files
file(GLOB SRC_FILES
    src/*.cpp
    src/Logic/*.cpp
    src/gui_qt/*.cpp
)

# Header files (optional, for IDEs)
file(GLOB HEADER_FILES
    src/*.h
    src/Logic/*.h
    src/gui_qt/*.h
)

qt_add_executable(NeonCycleExplorer
    ${SRC_FILES}
    ${HEADER_FILES}
)

qt_add_resources(NeonCycleExplorer src/resources.qrc)

target_include_directories(NeonCycleExplorer PRIVATE src src/Logic src/gui_qt)
target_link_libraries(NeonCycleExplorer PRIVATE Qt6::Widgets)