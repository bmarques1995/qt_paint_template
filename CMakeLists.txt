cmake_minimum_required(VERSION 3.20)

set(PROJ_NAME qt_paint_template)
project(${PROJ_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE PROJ_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "./src/include/*.hpp")
file(GLOB_RECURSE PROJ_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "./src/lib/*.cpp")

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
find_package(JsonCpp COMPONENTS  REQUIRED)
find_package(Eigen3 COMPONENTS Eigen REQUIRED)
find_library(D3D11_TARGET NAMES d3d11 d3dcompiler REQUIRED)

qt_standard_project_setup()

qt_add_executable(${PROJ_NAME} WIN32 ${PROJ_HDRS} ${PROJ_SRCS})
target_include_directories(${PROJ_NAME} PRIVATE "./src/include" "./install/include")
target_link_directories(${PROJ_NAME} PRIVATE "./install/lib")
target_compile_definitions(${PROJ_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
target_link_libraries(${PROJ_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets jsoncpp d3d11 d3dcompiler)
target_compile_definitions(${PROJ_NAME} PRIVATE QT_DEBUG_PLUGINS=1)

add_custom_target(copy_dlls ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/install/bin
            ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "copying DLLs")

add_custom_target(copy_plugins ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${PROJECT_SOURCE_DIR}/install/plugins
            ${CMAKE_CURRENT_BINARY_DIR}/../plugins
            COMMENT "copying Qt plugins")

add_dependencies(${PROJ_NAME} copy_dlls copy_plugins)