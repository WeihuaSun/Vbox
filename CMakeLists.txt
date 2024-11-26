cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER "g++")
project(SerVerifier)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)


set(SOURCES
    src/transaction/transaction.cpp
    src/graph/graph.cpp
    src/verifier/vbox.cpp
    src/verifier/leopard.cpp
    src/solver/constraint.cpp
    src/main.cpp
)

add_executable(SerVerifier ${SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic  -O3")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ")
set(CMAKE_BUILD_TYPE Release)
