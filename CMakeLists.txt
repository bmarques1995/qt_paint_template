cmake_minimum_required(VERSION 3.20)

set(PROJ_NAME qt_paint_template)
project(${PROJ_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE PROJ_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "./src/include/*.hpp")
file(GLOB_RECURSE PROJ_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "./src/lib/*.cpp")

add_executable(${PROJ_NAME} ${PROJ_HDRS} ${PROJ_SRCS})
target_include_directories(${PROJ_NAME} PRIVATE "./src/include")