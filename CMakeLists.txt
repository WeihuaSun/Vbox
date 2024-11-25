cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER "g++")
project(SerVerifier)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

# 源文件列表
set(SOURCES
    src/transaction/transaction.cpp
    src/graph/graph.cpp
    src/verifier/vbox.cpp
    src/verifier/leopard.cpp
    src/solver/constraint.cpp
    src/main.cpp
)

# 添加可执行文件
add_executable(SerVerifier ${SOURCES})



# 添加 -O3 优化选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic  -O3")

# 链接选项 - 也需要加上 -pg
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")

# 设置 Release 配置下的优化级别
set(CMAKE_BUILD_TYPE Release)
